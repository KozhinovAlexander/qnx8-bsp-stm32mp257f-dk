/*
 * io-sock sample driver code
 */

#ifndef DWMAC_STM32_H
#define DWMAC_STM32_H
#define RX_DESC_COUNT		1024
#define TX_DESC_COUNT		1024
#define TX_MAP_COUNT		TX_DESC_COUNT
#define TX_MAP_MAX_SEGS		1
#define WATCHDOG_TIMEOUT_SECS	5
#define SAMPLE_DESC_RING_ALIGN	2048
#define TX_QUEUE_LEN		4096
#define TX_DESC_SIZE		sizeof(struct sam_hwdesc)
#define RX_DESC_SIZE		sizeof(struct sam_hwdesc)
#define TX_DESC_RING_SIZE	(TX_DESC_SIZE * TX_DESC_COUNT)
#define RX_DESC_RING_SIZE	(RX_DESC_SIZE * RX_DESC_COUNT)

#define PHY_MODE_UNKNOWN	0x0
#define PHY_MODE_RMII		0x1
#define PHY_MODE_RGMII		0x2

/* macros used to access SYS_RES_MEMORY in sam_spec */
#define READ4(_sc, _reg)	bus_read_4((_sc)->res[0], _reg)
#define WRITE4(_sc, _reg, _val)	bus_write_4((_sc)->res[0], _reg, _val)

/* Sample defines used to service HW interrupts */
#define SAM_INTR_STATUS_REG	0x1000
#define SAM_INTR_RXREADY	(1 << 0)
#define SAM_INTR_TXDONE		(1 << 1)

/* Sample hardware descriptor.  Determined by the hardware. */
#define DESC_HW_OWN		(1 << 0)
#define DESC_LAST		(1 << 1)
struct sam_hwdesc {
	uint32_t	paddrl;
	uint32_t	paddrh;
	uint32_t	length;
	uint32_t	flags;
};

struct sam_bufmap {
	bus_dmamap_t	map;
	struct mbuf	*mbuf;
 };

struct sam_softc {
	struct resource		*res[2];
	device_t		dev;
	device_t		miibus;
	struct mii_data		*mii_softc;
	if_t			ifp;
	int			if_flags;
	struct mtx		mtx;
	void			*intr_cookie;
	struct callout		sam_callout;
	boolean_t		link_is_up;
	boolean_t		is_detaching;
	int			tx_watchdog_count;
	int			phy_mode;
	struct ether_addr	hwaddr;

	/* RX */
	bus_dma_tag_t		rxdesc_tag;
	bus_dmamap_t		rxdesc_map;
	struct sam_hwdesc	*rxdesc_ring;
	bus_addr_t		rxdesc_ring_paddr;
	bus_dma_tag_t		rxbuf_tag;
	struct sam_bufmap	rxbuf_map[RX_DESC_COUNT];
	uint32_t		rx_idx;

	/* TX */
	bus_dma_tag_t		txdesc_tag;
	bus_dmamap_t		txdesc_map;
	struct sam_hwdesc	*txdesc_ring;
	bus_addr_t		txdesc_ring_paddr;
	bus_dma_tag_t		txbuf_tag;
	struct sam_bufmap	txbuf_map[TX_DESC_COUNT];
	uint32_t		tx_mapcount;

	uint32_t		txbuf_map_head;
	uint32_t		txbuf_map_tail;

	struct mtx		tx_mtx;
	struct buf_ring		*tx_br;
	struct taskqueue	*tx_taskq;
	struct task		tx_task;
#ifdef INCLUDE_ETHERSWITCH
	struct etherswitch_info	es_info;
#endif
 };

/*
 * Define macros for debug logging.
 * A verbosity level, N, is used.
 */
extern int32_t g_debug;
#define	DEVPRINTFN(n, dev, fmt, ...) do { \
	if (g_debug >= n) { \
		device_printf(dev, "%s - " fmt, \
		    __func__ , ##__VA_ARGS__); \
	} \
} while (0)
#define DEVPRINTF(...) DEVPRINTFN(1, __VA_ARGS__)

#endif /* DWMAC_STM32_H */
