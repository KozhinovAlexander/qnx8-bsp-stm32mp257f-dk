/*
 * io-sock ofwbus/fdt sample driver
 */

#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/bus.h>

#include <sys/kernel.h>
#include <sys/lock.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/module.h>
#include <sys/mutex.h>
#include <sys/rman.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/taskqueue.h>
#include <net/bpf.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <net/if_dl.h>
#include <net/if_media.h>
#include <net/if_types.h>
#include <net/if_var.h>

#include <machine/bus.h>
#include <dev/fdt/fdt_common.h>
#include <dev/mii/mii.h>
#include <dev/mii/miivar.h>
#include <dev/ofw/ofw_bus.h>
#include <dev/ofw/ofw_bus_subr.h>
#include <qnx/qnx_modload.h>
#include <qnx/qnx_smmu.h>
#include "miibus_if.h"
/*
 * Include etherswitch callbacks in diag version of the driver.
 * Non-switch drivers can use the etherswitch framework as a debugging
 * tool to expose the ioctls used by the etherswitchcfg utility to
 * read HW registers. https://man.freebsd.org/cgi/man.cgi?query=
 * etherswitchcfg&sektion=8&manpath=FreeBSD+14.3-RELEASE+and+Ports
 */
#ifdef INVARIANTS
#define INCLUDE_ETHERSWITCH
#include <dev/etherswitch/etherswitch.h>
#include "etherswitch_if.h"
#endif
#include "sample.h"

#define SAMPLE_LOCK(sc) mtx_lock(&(sc)->mtx)
#define SAMPLE_UNLOCK(sc) mtx_unlock(&(sc)->mtx)
#define SAMPLE_ASSERT_LOCKED(sc) mtx_assert(&(sc)->mtx, MA_OWNED)
#define SAMPLE_ASSERT_UNLOCKED(sc) mtx_assert(&(sc)->mtx, MA_NOTOWNED)
#define SAMPLE_TX_LOCK(sc) mtx_lock(&(sc)->tx_mtx)
#define SAMPLE_TX_UNLOCK(sc) mtx_unlock(&(sc)->tx_mtx)
#define SAMPLE_ASSERT_TX_LOCKED(sc) mtx_assert(&(sc)->tx_mtx, MA_OWNED)
#define SAMPLE_ASSERT_TX_UNLOCKED(sc) mtx_assert(&(sc)->tx_mtx, MA_NOTOWNED)

#ifdef INVARIANTS
/* enable debug logs by default in the diag version of the driver */
int32_t g_debug = 1;
#else
int32_t g_debug = 0;
#endif
static SYSCTL_NODE(_hw, OID_AUTO, sam, CTLFLAG_RD | CTLFLAG_MPSAFE, 0,
    "sample driver global parameters");
SYSCTL_INT(_hw_sam, OID_AUTO, debug, CTLFLAG_RWTUN, &g_debug, 0,
    "Debug logging level (0 - 9)");

int drvr_ver = IOSOCK_VERSION_CUR;
SYSCTL_INT(_qnx_driver, OID_AUTO, sam, CTLFLAG_RD, &drvr_ver, 0,
    "Version");

/*
 * Array of compatibility strings that a device supported
 * by this driver will specify in the dtb.
 */
static struct ofw_compat_data compat_data[] = {
	{ "sam,sample-v1", 1 },
	{ "sam,sample-v2", 2 },
	{ "sam,sample-v3", 3 },
	{ NULL, 0 }
};

static struct resource_spec sam_spec[] = {
	{ SYS_RES_MEMORY, 0, RF_ACTIVE },
	{ SYS_RES_IRQ, 0, RF_ACTIVE },
	{ -1, 0 }
};

static bool
desc_owned(struct sam_hwdesc *desc)
{
	/* Checking descriptor ownership is hardware specifc */
	return (desc->flags & DESC_HW_OWN);
}

static uint32_t
next_txidx(uint32_t curidx)
{
	return ((curidx + 1) % TX_MAP_COUNT);
}

static uint32_t
next_rxidx(uint32_t curidx)
{
	return ((curidx + 1) % RX_DESC_COUNT);
}

static void
sam_txfinish_locked(struct sam_softc *sc)
{
	struct sam_bufmap *bmap;
	struct sam_hwdesc *desc;

	SAMPLE_ASSERT_TX_LOCKED(sc);

	bus_dmamap_sync(sc->txdesc_tag, sc->txdesc_map,
	    BUS_DMASYNC_POSTREAD | BUS_DMASYNC_POSTWRITE);
	while (sc->txbuf_map_tail != sc->txbuf_map_head) {
		bmap = &sc->txbuf_map[sc->txbuf_map_tail];
		desc = &sc->txdesc_ring[sc->txbuf_map_tail];
		/* assumes a single descriptor per map */
		if (desc_owned(desc)) {
			/* descriptor owned by hw, exit */
			break;
		}

		bus_dmamap_sync(sc->txbuf_tag, bmap->map,
		    BUS_DMASYNC_POSTWRITE);
		bus_dmamap_unload(sc->txbuf_tag, bmap->map);
		DEVPRINTFN(9, sc->dev, "Packet transmitted: %d bytes\n",
		    bmap->mbuf->m_len);
		m_freem(bmap->mbuf);
		bmap->mbuf = NULL;
		sc->tx_mapcount--;

		if_inc_counter(sc->ifp, IFCOUNTER_OPACKETS, 1);
		if_setdrvflagbits(sc->ifp, 0, IFF_DRV_OACTIVE);
		sc->txbuf_map_tail = next_txidx(sc->txbuf_map_tail);
	}
	/* stop the watchdog if there are no outstanding buffers */
	if (sc->txbuf_map_tail == sc->txbuf_map_head) {
		sc->tx_watchdog_count = 0;
	}
}

static void
sam_setup_txdesc(struct sam_softc *sc, int idx, bus_addr_t paddr,
    uint32_t len, uint32_t flags)
{
	/* setting up descriptors is HW dependent */
	sc->txdesc_ring[idx].paddrl = (uint32_t)paddr;
	sc->txdesc_ring[idx].paddrh = (uint32_t)(paddr >> 32);
	sc->txdesc_ring[idx].length = len;
	wmb();
	/*
	 * Indicating descriptor ownership is HW dependent.
	 * Use a memory barrier in this example to ensure the descriptor
	 * is fully written before transferring ownership.
	 */
	sc->txdesc_ring[idx].flags = flags;
	bus_dmamap_sync(sc->txdesc_tag, sc->txdesc_map,
	    BUS_DMASYNC_PREREAD | BUS_DMASYNC_PREWRITE);
}

static int
sam_setup_txbuf(struct sam_softc *sc, int idx, struct mbuf **mp)
{
	struct bus_dma_segment segs[TX_MAP_MAX_SEGS];
	int nsegs, error;
	struct mbuf * m;
	int desc_idx;

	error = bus_dmamap_load_mbuf_sg(sc->txbuf_tag, sc->txbuf_map[idx].map,
	    *mp, segs, &nsegs, 0);
	/*
	 * If bus_dmamap_load_mbuf_sg wasn't successful in mapping all
	 * segments, we will try to defrag it.
	 */
	if (error == EFBIG) {
		/* The map may be partially mapped from the first call. */
		bus_dmamap_unload(sc->txbuf_tag, sc->txbuf_map[idx].map);
		if ((m = m_defrag(*mp, M_NOWAIT)) == NULL)
			return (ENOMEM);
		*mp = m;
		error = bus_dmamap_load_mbuf_sg(sc->txbuf_tag,
		    sc->txbuf_map[idx].map, *mp, segs, &nsegs, 0);
	}
	if (error != 0) {
		bus_dmamap_unload(sc->txbuf_tag, sc->txbuf_map[idx].map);
		return (ENOMEM);
	}

	m = *mp;

	bus_dmamap_sync(sc->txbuf_tag, sc->txbuf_map[idx].map,
	    BUS_DMASYNC_PREWRITE);

	sc->txbuf_map[idx].mbuf = m;

	/*
	 * For simplicity, the sample only shows support for nseg = 1.
	 * (i.e. TX_MAP_MAX_SEGS = 1)
	 * In this case, the map index and descriptor index will be the same.
	 * How multiple segments are supported by the device
	 * descriptors is hardware dependent.
	 */
	desc_idx = idx;
	sam_setup_txdesc(sc, desc_idx, segs[0].ds_addr, segs[0].ds_len,
	    DESC_HW_OWN);

	return (0);
}

static void
sam_transmit_locked(struct sam_softc *sc)
{
	struct mbuf *m0;
	int error;
	int enqueued = 0;

	SAMPLE_ASSERT_TX_LOCKED(sc);
	if (((if_getdrvflags(sc->ifp) & IFF_DRV_RUNNING) == 0) ||
	    !sc->link_is_up) {
		return;
	}

	while(1) {
		if (sc->tx_mapcount > (TX_MAP_COUNT -1)) {
			if_setdrvflagbits(sc->ifp, IFF_DRV_OACTIVE, 0);
			sam_txfinish_locked(sc);
			break;
		}
		/*
		 * TODO: also check for available descriptors.
		 * The sample only uses a single descriptor per map,
		 * so checking mapcount is sufficient for the sample.
		 */

		m0 = drbr_peek(sc->ifp, sc->tx_br);
		if (m0 == NULL) {
			break;
		}

		error = sam_setup_txbuf(sc, sc->txbuf_map_head, &m0);
		if (error != 0) {
			DEVPRINTF(sc->dev, "setup_txbuf error:%d\n", error);
			drbr_putback(sc->ifp, sc->tx_br, m0);
			if_setdrvflagbits(sc->ifp, IFF_DRV_OACTIVE, 0);
		}

		DEVPRINTFN(9, sc->dev, "Packet sent to hw: %d bytes\n",
		    m0->m_len);
		BPF_MTAP(sc->ifp, m0);
		enqueued++;
		sc->txbuf_map_head = next_txidx(sc->txbuf_map_head);
		sc->tx_mapcount++;
		drbr_advance(sc->ifp, sc->tx_br);
	}
	if (enqueued != 0) {
		/* HW specific code to start TX, e.g. ring doorbell */

		/* Start watchdog */
		sc->tx_watchdog_count = WATCHDOG_TIMEOUT_SECS;
	}
	if (!drbr_empty(sc->ifp, sc->tx_br)) {
		taskqueue_enqueue(sc->tx_taskq, &sc->tx_task);
	}
}

static int
sam_transmit(if_t ifp, struct mbuf *m)
{
	struct sam_softc *sc;
	int ret, is_drbr_empty;

	sc = if_getsoftc(ifp);
	if(((if_getdrvflags(sc->ifp) & IFF_DRV_RUNNING) == 0) ||
	    !(sc->link_is_up)) {
		return ENETDOWN;
	}

	is_drbr_empty = drbr_empty(sc->ifp, sc->tx_br);
	ret = drbr_enqueue(ifp, sc->tx_br, m);
	if (ret != 0) {
		taskqueue_enqueue(sc->tx_taskq, &sc->tx_task);
		return (ret);
	}
	if (is_drbr_empty && mtx_trylock(&sc->tx_mtx)) {
		sam_transmit_locked(sc);
		SAMPLE_TX_UNLOCK(sc);
	} else {
		/* Wake up the task queue */
		taskqueue_enqueue(sc->tx_taskq, &sc->tx_task);
	}

	return ret;
}

static void
sam_transmit_deferred(void *arg, int pending)
{
	struct sam_softc *sc;

	sc = arg;

	while ((!drbr_empty(sc->ifp, sc->tx_br) &&
	    (if_getdrvflags(sc->ifp) & IFF_DRV_RUNNING) != 0)) {
		SAMPLE_TX_LOCK(sc);
		sam_transmit_locked(sc);
		SAMPLE_TX_UNLOCK(sc);
	}
}

static void
sam_get1paddr(void *arg, bus_dma_segment_t *segs, int nsegs, int error)
{
	if (error != 0) {
		return;
	}
	*(bus_addr_t *)arg = segs[0].ds_addr;
}

static void
sam_setup_rxfilter(struct sam_softc *sc)
{
	/* HW specific code here: enable HW filtering */
}

static struct mbuf *
sam_alloc_mbufcl(struct sam_softc *sc)
{
	struct mbuf *m;

	m = m_getcl(M_NOWAIT, MT_DATA, M_PKTHDR);
	if (m != NULL) {
		m->m_pkthdr.len = m->m_len = m->m_ext.ext_size;
	}

	return (m);
}

inline static void
sam_setup_rxdesc(struct sam_softc *sc, int idx, bus_addr_t paddr)
{
	/*
	 * Setting up descriptors and indicating descriptor ownership
	 * is hardware specific.
	 */
	sc->rxdesc_ring[idx].paddrh = (uint32_t)(paddr >> 32);
	sc->rxdesc_ring[idx].paddrl = (uint32_t)paddr;
	if (idx == (RX_DESC_COUNT -1)) {
		sc->rxdesc_ring[idx].flags |= DESC_LAST;
	}

	wmb();
	sc->rxdesc_ring[idx].flags = DESC_HW_OWN;
}

static int
sam_setup_rxbuf(struct sam_softc *sc, int idx, struct mbuf *m)
{
	struct bus_dma_segment seg;
	int error, nsegs;

	m_adj(m, ETHER_ALIGN);

	error = bus_dmamap_load_mbuf_sg(sc->rxbuf_tag, sc->rxbuf_map[idx].map,
	    m, &seg, &nsegs, 0);
	if (error != 0) {
		return (error);
	}

	KASSERT(nsegs == 1, ("%s: %d segments returned", __func__, nsegs));

	bus_dmamap_sync(sc->rxbuf_tag, sc->rxbuf_map[idx].map,
	    BUS_DMASYNC_PREREAD);

	sc->rxbuf_map[idx].mbuf = m;

	sam_setup_rxdesc(sc, idx, seg.ds_addr);

	return (0);
}

static struct mbuf *
sam_rxfinish_one(struct sam_softc *sc, struct sam_hwdesc *desc,
    struct sam_bufmap *map)
{
	struct mbuf *m, *m0;
	int len = 0;

	m = map->mbuf;

	/*
	 * HW specific code here: check length, return NULL if length doesn't
	 * make sense.
	 */

	/* Allocate new buffer */
	m0 = sam_alloc_mbufcl(sc);
	if (m0 == NULL) {
		/* no new mbuf available, recycle old */
		DEVPRINTFN(8, sc->dev, "Packet dropped, recycling mbuf\n");
		if_inc_counter(sc->ifp, IFCOUNTER_IQDROPS, 1);
		return (NULL);
	}
	/* do dmasync for newly received packet */
	bus_dmamap_sync(sc->rxbuf_tag, map->map, BUS_DMASYNC_POSTREAD);
	bus_dmamap_unload(sc->rxbuf_tag, map->map);

	/* Received packet is valid, process it. */
	m->m_pkthdr.rcvif = sc->ifp;
	m->m_pkthdr.len = len;
	m->m_len = len;

	if_inc_counter(sc->ifp, IFCOUNTER_IPACKETS, 1);
	DEVPRINTFN(9, sc->dev, "Packet received: %d bytes.\n", len);

	SAMPLE_UNLOCK(sc);
	if_input(sc->ifp, m);
	SAMPLE_LOCK(sc);

	return (m0);
}

static void
sam_rxfinish_locked(struct sam_softc *sc)
{
	struct sam_hwdesc *desc = NULL;
	struct mbuf *m;
	uint32_t idx;

	SAMPLE_ASSERT_LOCKED(sc);
	bus_dmamap_sync(sc->rxdesc_tag, sc->rxdesc_map,
	    BUS_DMASYNC_POSTWRITE | BUS_DMASYNC_POSTREAD);
	while (1) {
		idx = sc->rx_idx;
		if (desc_owned(sc->rxdesc_ring + idx)) {
			/* descriptor owned by hw, exit */
			break;
		}
		m = sam_rxfinish_one(sc, desc, sc->rxbuf_map + idx);
		if (m == NULL) {
			/* error, reuse existing map */
			wmb();
			sc->rxdesc_ring[idx].flags |= DESC_HW_OWN;
			wmb();
		} else {
			int error;
			error = sam_setup_rxbuf(sc, idx, m);
			if (error != 0) {
				/* rx ring must be contigious, try a reset? */
				panic("setup_rxbuf failed:%d\n", error);
			}
		}
		sc->rx_idx = next_rxidx(sc->rx_idx);
	}
	bus_dmamap_sync(sc->rxdesc_tag, sc->rxdesc_map,
	    BUS_DMASYNC_PREREAD | BUS_DMASYNC_PREWRITE);
}

static void
sam_intr(void *arg)
{
	struct sam_softc *sc;
	uint32_t intr_status;

	sc = arg;
	SAMPLE_LOCK(sc);
	/*
	 * HW specific code to decode the interrupt.
	 */
	intr_status = READ4(sc, SAM_INTR_STATUS_REG);

	if (intr_status & SAM_INTR_RXREADY) {
		DEVPRINTFN(9,sc->dev, "RXREADY interrupt\n");
		sam_rxfinish_locked(sc);
	}

	if (intr_status & SAM_INTR_TXDONE) {
		DEVPRINTFN(9,sc->dev, "TXDONE interrupt\n");
		sam_txfinish_locked(sc);
		sam_transmit_locked(sc);
	}

	SAMPLE_UNLOCK(sc);
}

static void
sam_media_status(if_t ifp, struct ifmediareq *ifmr)
{
	struct sam_softc *sc;
	struct mii_data *mii;

	sc = if_getsoftc(ifp);
	mii = sc->mii_softc;

	SAMPLE_LOCK(sc);
	mii_pollstat(mii);
	ifmr->ifm_active = mii->mii_media_active;
	ifmr->ifm_status = mii->mii_media_status;
	SAMPLE_UNLOCK(sc);
}

static int
sam_media_change(if_t ifp)
{
	struct sam_softc *sc;
	int error;

	sc = if_getsoftc(ifp);
	SAMPLE_LOCK(sc);
	error = mii_mediachg(sc->mii_softc);
	SAMPLE_UNLOCK(sc);
	return (error);
}

static void
sam_dma_disable(struct sam_softc *sc)
{
	/* Disable hardware DMA */
}

static int
tx_dma_init(struct sam_softc *sc)
{
	/* Initialize hardware for Tx */

	/* Initialize buffer indices */
	sc->txbuf_map_head = 0;
	sc->txbuf_map_tail = 0;

	return (0);
}

static int
rx_dma_init(struct sam_softc *sc)
{
	struct mbuf *m;
	int error;
	int idx;

	/* Initialize hardware for Rx */

	/* Initialize buffer index */
	sc->rx_idx = 0;

	/* Populate Rx ring */
	for (idx = 0; idx < RX_DESC_COUNT; idx++) {
		if ((m = sam_alloc_mbufcl(sc)) == NULL) {
			device_printf(sc->dev, "failed to alloc mbuf\n");
			error = ENOMEM;
			break;
		}
		if ((error = sam_setup_rxbuf(sc, idx, m)) != 0) {
			device_printf(sc->dev,
			    "failed to create new RX buffer\n");
			break;
		}
	}
	return (error);
}

static int
setup_dma(struct sam_softc *sc)
{
	int error;
	int idx;

	/*
	 * Set up TX descriptor ring, descriptors, and dma maps.
	 *
	 * See the FreeBSD manual pages for more information about
	 * the function parameters fo bus_dma_tag_create.
	 * https://man.freebsd.org/cgi/man.cgi?query=
	 * bus_dma_tag_create&sektion=9&manpath=FreeBSD+14.3-RELEASE+and+Ports
	 *
	 * For devices that have restrictions on what memory ranges cannot be
	 * used for DMA, lowaddr may need to be diffferent than what is shown
	 * below.  e.g.  for devices that cannot use memory above 4G for DMA,
	 * lowaddr would be set to BUS_SPACE_MAXADDR_32BIT.
	 *
	 */
	error = bus_dma_tag_create(
	    bus_get_dma_tag(sc->dev),   /* Parent tag. */
	    SAMPLE_DESC_RING_ALIGN, 0,  /* alignment, boundary */
	    BUS_SPACE_MAXADDR,          /* lowaddr */
	    BUS_SPACE_MAXADDR,          /* highaddr */
	    NULL, NULL,                 /* filter, filterarg */
	    TX_DESC_RING_SIZE, 1,       /* maxsize, nsegments */
	    TX_DESC_RING_SIZE,          /* maxsegsize */
	    0,                          /* flags */
	    NULL, NULL,                 /* lockfunc, lockarg */
	    &sc->txdesc_tag);           /* address of new tag */
	if (error != 0) {
		device_printf(sc->dev, "failed to create TX ring DMA tag\n");
		goto out;
	}

	error = bus_dmamem_alloc(sc->txdesc_tag, (void**)&sc->txdesc_ring,
	    BUS_DMA_COHERENT | BUS_DMA_WAITOK | BUS_DMA_ZERO,
	    &sc->txdesc_map);
	if (error != 0) {
		device_printf(sc->dev, "failed to allocate TX desc ring\n");
		goto out;
	}

	error = bus_dmamap_load(sc->txdesc_tag, sc->txdesc_map,
	    sc->txdesc_ring, TX_DESC_RING_SIZE, sam_get1paddr,
	    &sc->txdesc_ring_paddr, 0);
	if (error != 0) {
		device_printf(sc->dev, "failed to load TX desc ring mapp\n");
		goto out;
	}

	error = bus_dma_tag_create(
	    bus_get_dma_tag(sc->dev),    /* Parent tag. */
	    1, 0,                        /* alignment, boundary */
	    BUS_SPACE_MAXADDR,           /* lowaddr */
	    BUS_SPACE_MAXADDR,           /* highaddr */
	    NULL, NULL,                  /* filter, filterarg */
	    MCLBYTES,                    /* max size */
	    TX_MAP_MAX_SEGS,             /* nsegments */
	    MCLBYTES,                    /* maxsegsize */
	    0,                           /* flags */
	    NULL, NULL,                  /* lockfunc, lockarg */
	    &sc->txbuf_tag);             /* address of new tag */
	if (error != 0) {
		device_printf(sc->dev, "failed to create TX ring DMA tag\n");
		goto out;
	}

	for (idx = 0; idx < TX_MAP_COUNT; idx++) {
		error = bus_dmamap_create(sc->txbuf_tag, BUS_DMA_COHERENT,
		    &sc->txbuf_map[idx].map);
		if (error != 0) {
			device_printf(sc->dev,
			    "failed to create TX buffer DMA map\n");
			goto out;
		}
	}
	memset(sc->txdesc_ring, 0, TX_DESC_RING_SIZE);
	/*
	 * Set up RX descriptor ring, descriptors, dma maps.
	 */
	error = bus_dma_tag_create(
	    bus_get_dma_tag(sc->dev),    /* Parent tag. */
	    SAMPLE_DESC_RING_ALIGN, 0,   /* alignment, boundary */
	    BUS_SPACE_MAXADDR,           /* lowaddr */
	    BUS_SPACE_MAXADDR,           /* highaddr */
	    NULL, NULL,                  /* filter, filterarg */
	    RX_DESC_RING_SIZE, 1,        /* maxsize, nsegments */
	    RX_DESC_RING_SIZE,           /* maxsegsize */
	    0,                           /* flags */
	    NULL, NULL,                  /* lockfunc, lockarg */
	    &sc->rxdesc_tag);            /* address of new tag */
	if (error != 0) {
		device_printf(sc->dev, "failed to create RX ring DMA tag\n");
		goto out;
	}

	error = bus_dmamem_alloc(sc->rxdesc_tag, (void **)&sc->rxdesc_ring,
	    BUS_DMA_COHERENT | BUS_DMA_WAITOK |
	    BUS_DMA_ZERO | BUS_DMA_NOCACHE,
	    &sc->rxdesc_map);
	if (error != 0) {
		device_printf(sc->dev, "failed to allocate RX desc ring\n");
		goto out;
	}

	error = bus_dmamap_load(sc->rxdesc_tag, sc->rxdesc_map,
	    sc->rxdesc_ring, RX_DESC_RING_SIZE, sam_get1paddr,
	    &sc->rxdesc_ring_paddr, 0);
	if (error != 0) {
		device_printf(sc->dev, "failed to load RX desc ring map\n");
		goto out;
	}

	error = bus_dma_tag_create(
	    bus_get_dma_tag(sc->dev),    /* Parent tag. */
	    1, 0,                        /* alignment, boundary */
	    BUS_SPACE_MAXADDR,           /* lowaddr */
	    BUS_SPACE_MAXADDR,           /* highaddr */
	    NULL, NULL,                  /* filter, filterarg */
	    MCLBYTES, 1,                 /* maxsize, nsegments */
	    MCLBYTES,                    /* maxsegsize */
	    0,                           /* flags */
	    NULL, NULL,                  /* lockfunc, lockarg */
	    &sc->rxbuf_tag);             /* address of new tag */
	if (error != 0) {
		device_printf(sc->dev, "failed to create RX buf DMA tag\n");
		goto out;
	}

	for (idx = 0; idx < RX_DESC_COUNT; idx++) {
		error = bus_dmamap_create(sc->rxbuf_tag, BUS_DMA_COHERENT,
		    &sc->rxbuf_map[idx].map);
		if (error != 0) {
			device_printf(sc->dev,
			    "failed to create RX buffer DMA map\n");
			goto out;
		}
	}

out:
	if (error != 0) {
		return (ENXIO);
	}
	return (0);
}

static int
sam_reset(struct sam_softc *sc)
{
	/* HW specific code here: Reset the controller */
	return (0);
}

static int
sam_mac_init(struct sam_softc *sc)
{
	/* HW specific code here: Initialize MAC registers */
	return (0);
}

static int
sam_mac_disable(struct sam_softc *sc)
{
	/* HW specific code here: Set MAC registers for stop */
	return (0);
}

static void
sam_get_hwaddr(struct sam_softc *sc)
{
	phandle_t node;

	node = ofw_bus_get_node(sc->dev);
	if (OF_getprop(node, "mac-address", sc->hwaddr.octet,
	    ETHER_ADDR_LEN) != -1 ||
	    OF_getprop(node, "local-mac-address", sc->hwaddr.octet,
	    ETHER_ADDR_LEN) != -1)
		return;
	DEVPRINTF(sc->dev, "No mac address found in fdt.\n");

	/* generate a random mac address */
	ether_gen_addr(sc->ifp, &sc->hwaddr);
}

/* miibus callbacks */
static int
sam_miibus_read_reg(device_t dev, int phy, int reg)
{
	/* HW specific code here: read phy register here */
	return 0xdeadbeef;
}

static int
sam_miibus_write_reg(device_t dev, int phy, int reg, int val)
{
	/* HW specific code here: write phy register here */
	return 0;
}

/*
* miibus_readextreg/writeextreg must be used to read/write the
* registers at the Clause45 only PHY devices or can be used to
* read/write the extended registers at some of the Clause22 PHY devices.
*/
static int
sam_miibus_readextreg(device_t dev, int phyad, int devad, int regad)
{
	/* HW specific code here: read phy register here */
	return 0xdeadbeef;
}

static int
sam_miibus_writeextreg(device_t dev, int phyad, int devad, int regad, int data)
{
	/* HW specific code here: write phy register here */
	return 0;
}

static void
sam_miibus_statchg(device_t dev)
{
	struct sam_softc *sc;
	struct mii_data *mii;

	/*
	 * Called by the MII bus driver when the PHY establishes
	 * link to set the MAC interface registers.
	 */

	sc = device_get_softc(dev);
	SAMPLE_ASSERT_LOCKED(sc);

	mii = sc->mii_softc;

	if (mii->mii_media_status & IFM_ACTIVE) {
		sc->link_is_up = true;
	} else {
		sc->link_is_up = false;
	}

	switch (IFM_SUBTYPE(mii->mii_media_active)) {
	case IFM_1000_T:
	case IFM_1000_SX:
	case IFM_100_TX:
	case IFM_100_T2:
	case IFM_10_T:
	case IFM_100_T:
		/* HW Specific code here: set speed */
		break;
	case IFM_NONE:
		sc->link_is_up = false;
		return;
	default:
		sc->link_is_up = false;
		device_printf(dev, "Unsupported media %u\n",
		    IFM_SUBTYPE(mii->mii_media_active));
		return;
	}

	if ((IFM_OPTIONS(mii->mii_media_active) & IFM_FDX) != 0) {
		/* HW Specific code here: set full duplex */
	}
	else {
		/* HW Specific code here: clear full duplex */
	}

	/* HW specific code here: implement media specific HW initialization */

}

#ifdef INCLUDE_ETHERSWITCH
/*
 * Etherswitch callbacks. These could be used in a debug version of
 * of a driver to read HW registers with the etherwitchcfg utility.
 */
static etherswitch_info_t *
sam_es_getinfo(device_t dev)
{
	struct sam_softc *sc;

	sc = device_get_softc(dev);

	sc->es_info.es_nports = 1;
	strncpy(sc->es_info.es_name, "Sample Ethernet MAC Driver",
	    ETHERSWITCH_NAMEMAX);

	return (&sc->es_info);
}

static int
sam_es_readreg(device_t dev, int reg)
{
	struct sam_softc *sc;
	uint32_t val;

	sc = device_get_softc(dev);

	if (reg > bus_get_resource_count(sc->dev, SYS_RES_MEMORY, 0)) {
		device_printf(dev, "%s - Register out of range\n", __func__);
		return 0;
	}

	val = READ4(sc, reg);

	return (val);
}
#endif

static void
sam_tick(void *arg)
{
	struct sam_softc *sc;
	int link_was_up;

	sc = arg;

	SAMPLE_ASSERT_LOCKED(sc);

	if (!(if_getdrvflags(sc->ifp) & IFF_DRV_RUNNING)) {
		return;
	}

	/*
	 * Typical tx watchdog.  If this fires it indicates that we enqueued
	 * packets for output and never got a txdone interrupt.
	 */
	if (sc->tx_watchdog_count > 0) {
		if (--sc->tx_watchdog_count == 0) {
			sam_txfinish_locked(sc);
		}
	}

	/* Check the media status. */
	link_was_up = sc->link_is_up;
	mii_tick(sc->mii_softc);
	if (sc->link_is_up && !link_was_up) {
		taskqueue_enqueue(sc->tx_taskq, &sc->tx_task);
	}

	/* Schedule another check one second from now. */
	callout_reset(&sc->sam_callout, hz, sam_tick, sc);
}

static void
sam_init_locked(struct sam_softc *sc)
{
	SAMPLE_ASSERT_LOCKED(sc);

	if (if_getdrvflags(sc->ifp) & IFF_DRV_RUNNING) {
		return;
	}

	if_setdrvflagbits(sc->ifp, IFF_DRV_RUNNING, IFF_DRV_OACTIVE);

	sam_setup_rxfilter(sc);

	sam_mac_init(sc);
	rx_dma_init(sc);
	tx_dma_init(sc);

	/*
	 * Call mii_mediachg() which will call back into sam_miibus_statchg()
	 * to set up the remaining config registers based on current media.
	 */
	mii_mediachg(sc->mii_softc);
	callout_reset(&sc->sam_callout, hz, sam_tick, sc);
}

static void
sam_stop_locked(struct sam_softc *sc)
{
	int i;

	SAMPLE_ASSERT_LOCKED(sc);

	/* clear the IFF_DRV_RUNNING flag */
	if_setdrvflagbits(sc->ifp, 0, IFF_DRV_RUNNING);
	sc->tx_watchdog_count = 0;

	callout_stop(&sc->sam_callout);

	/* HW specific code here: stop HW */
	sam_dma_disable(sc);
	sam_mac_disable(sc);

	wmb();
	/* Clear the tx ring buffer */
	for (i = 0; i < TX_DESC_COUNT; i++) {
		if (sc->txbuf_map[i].mbuf != NULL){
			bus_dmamap_sync(sc->txbuf_tag, sc->txbuf_map[i].map,
				BUS_DMASYNC_POSTWRITE);
			bus_dmamap_unload(sc->txbuf_tag, sc->txbuf_map[i].map);
			m_freem(sc->txbuf_map[i].mbuf);
			sc->txbuf_map[i].mbuf = NULL;
		}
	}

	/* Clear the rx ring buffer */
	for (i = 0; i < RX_DESC_COUNT; i++) {
		if (sc->rxbuf_map[i].mbuf != NULL) {
			bus_dmamap_sync(sc->rxbuf_tag, sc->rxbuf_map[i].map,
			    BUS_DMASYNC_POSTREAD | BUS_DMASYNC_POSTWRITE);
			bus_dmamap_unload(sc->rxbuf_tag, sc->rxbuf_map[i].map);
			m_freem(sc->rxbuf_map[i].mbuf);
			sc->rxbuf_map[i].mbuf = NULL;
		}
	}
}

static int
sam_ioctl(if_t ifp, u_long cmd, caddr_t data)
{
	struct sam_softc *sc;
	struct mii_data *mii;
	struct ifreq *ifr;
	int mask, error;

	sc = if_getsoftc(ifp);
	ifr = (struct ifreq *)data;
	error = 0;
	switch (cmd) {
	case SIOCSIFFLAGS:
		SAMPLE_LOCK(sc);
		if (if_getflags(ifp) & IFF_UP) {
			if (if_getdrvflags(ifp) & IFF_DRV_RUNNING) {
				if ((if_getflags(ifp) ^ sc->if_flags) &
				    (IFF_PROMISC | IFF_ALLMULTI)) {
					sam_setup_rxfilter(sc);
				}
			} else {
				if (!sc->is_detaching) {
					sam_init_locked(sc);
				}
			}
		} else {
			if (if_getdrvflags(ifp) & IFF_DRV_RUNNING) {
				sam_stop_locked(sc);
			}
		}
		sc->if_flags = if_getflags(ifp);
		SAMPLE_UNLOCK(sc);
		break;
	case SIOCADDMULTI:
	case SIOCDELMULTI:
		if (if_getdrvflags(ifp) & IFF_DRV_RUNNING) {
			SAMPLE_LOCK(sc);
			sam_setup_rxfilter(sc);
			SAMPLE_UNLOCK(sc);
		}
		break;
	case SIOCSIFMEDIA:
	case SIOCGIFMEDIA:
		mii = sc->mii_softc;
		error = ifmedia_ioctl(ifp, ifr, &mii->mii_media, cmd);
		break;
	case SIOCSIFCAP:
		mask = if_getcapenable(ifp) ^ ifr->ifr_reqcap;
		if (mask & IFCAP_VLAN_MTU) {
			if_togglecapenable(ifp, IFCAP_VLAN_MTU);
		}
		/*
		 * HW specific code here for additional capabilities,
		 * such as HW offload features.
		 */
		break;

	default:
		error = ether_ioctl(ifp, cmd, data);
		break;
	}
	return (error);
}

static void
sam_qflush(if_t ifp)
{
	struct sam_softc *sc;

	sc = if_getsoftc(ifp);
	if (!drbr_empty(ifp, sc->tx_br)) {
		SAMPLE_TX_LOCK(sc);
		drbr_flush(ifp, sc->tx_br);
		SAMPLE_TX_UNLOCK(sc);
	}

	if_qflush(ifp);
}

static void
sam_init(void *if_softc)
{
	struct sam_softc *sc;

	sc = if_softc;
	SAMPLE_LOCK(sc);
	sam_init_locked(sc);
	SAMPLE_UNLOCK(sc);
}

static int
sam_probe(device_t dev)
{
	if (!ofw_bus_status_okay(dev)) {
		return (ENXIO);
	}

	/*
	 * Check device's compatibility string against list of
	 * supported devices
	 */
	if (ofw_bus_search_compatible(dev, compat_data)->ocd_data == 0) {
		return (ENXIO);
	}
	device_set_desc(dev, "Sample Ethernet Controller");

	return (BUS_PROBE_DEFAULT);
}

static int
sam_detach(device_t dev)
{
	struct sam_softc *sc;
	bus_dmamap_t map;
	int idx;

	sc = device_get_softc(dev);

	SAMPLE_LOCK(sc);
	sc->is_detaching = true;
	sam_stop_locked(sc);
	SAMPLE_UNLOCK(sc);
	callout_drain(&sc->sam_callout);
	ether_ifdetach(sc->ifp);

	/* Detach the miibus */
	if (sc->miibus != NULL) {
		device_delete_child(dev, sc->miibus);
	}
	bus_generic_detach(dev);

	if (sc->ifp != NULL) {
		if_free(sc->ifp);
		sc->ifp = NULL;
	}

	/* Clean up RX DMA resources and free mbufs. */
	for (idx = 0; idx < RX_DESC_COUNT; ++idx) {
		if ((map = sc->rxbuf_map[idx].map) != NULL) {
			bus_dmamap_unload(sc->rxbuf_tag, map);
			bus_dmamap_destroy(sc->rxbuf_tag, map);
			m_freem(sc->rxbuf_map[idx].mbuf);
		}
	}
	if (sc->rxbuf_tag != NULL) {
		bus_dma_tag_destroy(sc->rxbuf_tag);
	}
	if (sc->rxdesc_map != NULL) {
		bus_dmamap_unload(sc->rxdesc_tag, sc->rxdesc_map);
		bus_dmamem_free(sc->rxdesc_tag, sc->rxdesc_ring,
		    sc->rxdesc_map);
	}
	if (sc->rxdesc_tag != NULL) {
		bus_dma_tag_destroy(sc->rxdesc_tag);
	}

	/* Clean up TX DMA resources. */
	for (idx = 0; idx < TX_MAP_COUNT; ++idx) {
		if ((map = sc->txbuf_map[idx].map) != NULL) {
			/* TX maps are already unloaded. */
			bus_dmamap_destroy(sc->txbuf_tag, map);
		}
	}
	if (sc->txbuf_tag != NULL) {
		bus_dma_tag_destroy(sc->txbuf_tag);
	}
	if (sc->txdesc_map != NULL) {
		bus_dmamap_unload(sc->txdesc_tag, sc->txdesc_map);
		bus_dmamem_free(sc->txdesc_tag, sc->txdesc_ring,
		sc->txdesc_map);
	}
	if (sc->txdesc_tag != NULL) {
		bus_dma_tag_destroy(sc->txdesc_tag);
	}

	/* Release bus resources. */
	del_mmio_smmu(rman_get_start(sc->res[0]), rman_get_size(sc->res[0]));
	bus_release_resources(dev, sam_spec, sc->res);

	SAMPLE_TX_LOCK(sc);
	if (sc->tx_taskq) {
		taskqueue_drain(sc->tx_taskq, &sc->tx_task);
		taskqueue_free(sc->tx_taskq);
		sc->tx_taskq = NULL;
	}
	if (sc->tx_br) {
		buf_ring_free(sc->tx_br, M_DEVBUF);
		sc->tx_br = NULL;
	}
	mtx_destroy(&sc->tx_mtx);

	if (sc->intr_cookie != NULL) {
		bus_teardown_intr(dev, sc->res[1], sc->intr_cookie);
		sc->intr_cookie = NULL;
	}

	mtx_destroy(&sc->mtx);
	return (0);
}

static int
sam_attach(device_t dev)
{
	struct sam_softc *sc;
	if_t ifp;
	int error, phynum;
	char *phy_mode;
	phandle_t node;
	void *dummy;
	char td_name[32];
	int mii_flags;

	sc = device_get_softc(dev);

	node = ofw_bus_get_node(dev);
	if ((node = ofw_bus_get_node(dev)) == -1) {
		device_printf(dev, "failed to find ofw bus node\n");
		error = ENXIO;
		goto done;
	}

	/*
	 * HW specific code here: just an example of how to read some
	 * property from DTB.
	 */
	if (OF_getprop_alloc(node, "phy-mode", (void **)&phy_mode)) {
		if (strcmp(phy_mode, "rgmii") == 0) {
			sc->phy_mode = PHY_MODE_RGMII;
		}
		if (strcmp(phy_mode, "rmii") == 0) {
			sc->phy_mode = PHY_MODE_RMII;
		}
		OF_prop_free(phy_mode);
	}

	if (bus_alloc_resources(dev, sam_spec, sc->res)) {
		device_printf(dev, "failed to allocate resources\n");
		error = ENXIO;
		goto done;
	}
	add_mmio_smmu(rman_get_start(sc->res[0]), rman_get_size(sc->res[0]));

	/* Reset the HW if needed. */
	if (sam_reset(sc) != 0) {
		device_printf(dev, "failed to reset the HW\n");
		error = ENXIO;
		goto done;
	}

	mtx_init(&sc->tx_mtx, td_name, NULL, MTX_DEF);

	/* buf_ring_alloc requires a size that is a power of 2 */
	sc->tx_br = buf_ring_alloc(TX_QUEUE_LEN, M_DEVBUF, M_NOWAIT,
	    &sc->tx_mtx);
	if (sc->tx_br == NULL) {
		device_printf(dev, "buf_ring_alloc failed");
		error = ENOMEM;
		goto done;
	}

	TASK_INIT(&sc->tx_task, 0, sam_transmit_deferred, sc);
	sc->tx_taskq = taskqueue_create(td_name, M_NOWAIT,
	    taskqueue_thread_enqueue, &sc->tx_taskq);
	if (sc->tx_taskq == NULL) {
		device_printf(dev, "taskqueue_create failed");
		error = ENOMEM;
		goto done;
	}
	taskqueue_start_threads(&sc->tx_taskq, 1, PI_NET, td_name);

	if (setup_dma(sc) != 0) {
		error = ENXIO;
		goto done;
	}

	mtx_init(&sc->mtx, device_get_nameunit(sc->dev),
	    MTX_NETWORK_LOCK, MTX_DEF);

	callout_init_mtx(&sc->sam_callout, &sc->mtx, 0);

	/* Setup interrupt handler. */
	error = bus_setup_intr(dev, sc->res[1], INTR_TYPE_NET | INTR_MPSAFE,
	    NULL, sam_intr, sc, &sc->intr_cookie);
	if (error != 0) {
		device_printf(dev, "failed to setup interrupt handler\n");
		error = ENXIO;
		goto done;
	}

	/* Set up the ethernet interface. */
	sc->ifp = ifp = if_alloc(IFT_ETHER);

	if_setsoftc(ifp, sc);
	if_initname(ifp, device_get_name(dev), device_get_unit(dev));
	if_setflags(ifp, IFF_BROADCAST | IFF_SIMPLEX | IFF_MULTICAST);
	if_setcapabilities(ifp, IFCAP_VLAN_MTU);
	if_setcapenable(ifp, if_getcapabilities(ifp));
	if_settransmitfn(ifp, sam_transmit);
	if_setqflushfn(ifp, sam_qflush);
	if_setioctlfn(ifp, sam_ioctl);
	if_setinitfn(ifp, sam_init);
	if_setsendqlen(ifp, TX_QUEUE_LEN - 1);
	if_setsendqready(ifp);
	if_setifheaderlen (ifp, sizeof(struct ether_vlan_header));

	if (fdt_get_phyaddr(node, dev, &phynum, &dummy) != 0) {
		phynum = MII_PHY_ANY;
	}
	mii_flags = MIIF_FORCEANEG;
	/*
	* If the PHY device only supports Clause45, use MIIF_CLAUSE45 flag to
	* force the MII framework to use Clause 45 registers to find the PHY device
	* mii_flags |= MIIF_CLAUSE45;
	*/

	/* Attach the mii driver. */
	error = mii_attach(dev, &sc->miibus, ifp, sam_media_change,
	    sam_media_status, BMSR_DEFCAPMASK, phynum,
	    MII_OFFSET_ANY, mii_flags);
	if (error != 0) {
		device_printf(dev, "PHY attach failed\n");
		error = ENXIO;
		goto done;
	}
	sc->mii_softc = device_get_softc(sc->miibus);
	sam_get_hwaddr(sc);
	/* All ready to run, attach the ethernet interface. */
	ether_ifattach(ifp, sc->hwaddr.octet);

done:
	if (error != 0) {
		device_printf(dev, "failed to attach, error:%d\n", error);
		sam_detach(dev);
	}

	return error;
}

static int
sam_shutdown(device_t dev)
{
	/*
	 * HW Specific code here.
	 * Shutdown is called when the io-sock process terminates.
	 * Unlike sam_detach, sam_shutdown is not intended to be a
	 * graceful stop. This method should put the hardware in a safe
	 * state (e.g. disable HW DMA) in the simplest manner possible.
	 * There should be no blocking calls and this function should return
	 * as quickly as possible.
	 */

	return 0;
}

/* Driver callback registration */
static device_method_t sam_methods[] = {
	/* device callbacks */
	DEVMETHOD(device_probe, sam_probe),
	DEVMETHOD(device_attach, sam_attach),
	DEVMETHOD(device_detach, sam_detach),
	DEVMETHOD(device_shutdown, sam_shutdown),

	/* miibus callbacks */
	DEVMETHOD(miibus_readreg, sam_miibus_read_reg),
	DEVMETHOD(miibus_writereg, sam_miibus_write_reg),
	DEVMETHOD(miibus_readextreg, sam_miibus_readextreg),
	DEVMETHOD(miibus_writeextreg, sam_miibus_writeextreg),
	DEVMETHOD(miibus_statchg, sam_miibus_statchg),

#ifdef INCLUDE_ETHERSWITCH
	/* etherswitch callbacks */
	DEVMETHOD(etherswitch_getinfo, sam_es_getinfo),
	DEVMETHOD(etherswitch_readreg, sam_es_readreg),
	DEVMETHOD(etherswitch_readphyreg, sam_miibus_read_reg),
#endif

	DEVMETHOD_END
};

driver_t sample_driver = {
	"sam",
	sam_methods,
	sizeof(struct sam_softc),
};

static devclass_t sam_devclass;

/*
 * The sample driver is showing how the device name and module name
 * can be different.
 * e.g.  module name is sample, and device name is sam.
 * Most FreeBSD drivers will use the same name for the driver,
 * device and the module.
 */
DRIVER_MODULE(sample, simplebus, sample_driver, sam_devclass, 0, 0);
/*
 * Since the second parameter of the DRIVER_MODULE macro is the bus name,
 * the driver device name is used in the following and not the sample
 * driver module name.
 * DRIVER_MODULE(modname, busname, driver_t driver, modeventhand_t evh,
 *     void *arg);
 */
DRIVER_MODULE(miibus, sam, miibus_driver, miibus_devclass, 0, 0);
#ifdef INCLUDE_ETHERSWITCH
DRIVER_MODULE(etherswitch, sam, etherswitch_driver, etherswitch_devclass, 0, 0);
#endif
MODULE_DEPEND(sample, ether, 1, 1, 1);
MODULE_DEPEND(sample, miibus, 1, 1, 1);
#ifdef INCLUDE_ETHERSWITCH
MODULE_DEPEND(sample, etherswitch, 1, 1, 1);
#endif
MODULE_VERSION(sample, 1);

struct _iosock_module_version iosock_module_version =
    IOSOCK_MODULE_VER_SYM_INIT;

static void
sam_uninit(void *arg)
{
}
/*
 * If code is added to sam_uninit, then SI_SUB_DUMMY needs to be
 * changed to SI_SUB_DRIVERS.  With SI_SUB_DUMMY, sam_uninit does
 * not get called.
 */
SYSUNINIT(sam_uninit, SI_SUB_DUMMY, SI_ORDER_ANY, sam_uninit, NULL);
