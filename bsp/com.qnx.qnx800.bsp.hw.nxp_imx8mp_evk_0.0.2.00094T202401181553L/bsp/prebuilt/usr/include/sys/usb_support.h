/*
 * $QNXLicenseC:
 * Copyright 2016, QNX Software Systems. All Rights Reserved.
 *
 * This source code may contain confidential information of QNX Software
 * Systems (QSS) and its licensors.  Any use, reproduction, modification,
 * disclosure, distribution or transfer of this software, or any software
 * that includes or is based upon any of this code, is prohibited unless
 * expressly authorized by QSS by written agreement.  For more information
 * (including whether this source code file has been published) please
 * email licensing@qnx.com. $
*/

#ifndef __USB_SUPPORT_H_INCLUDED
#define __USB_SUPPORT_H_INCLUDED

#include <stdarg.h>
#include <stdint.h>
#include <pthread.h>
#include <pci/pci.h>
#include <sys/slogcodes.h>
#include <smmu.h>
#include <sys/usb_smmu.h>

#define USB_PRIORITY					21
#define USB_STACK_SIZE					32768
#define USB_TSTATE_CREATING				~0
#define USB_THREAD_INTR_PULSE			1
#define	USB_THREAD_EXIT_PULSE			0xffffffff
#define USB_PULSE_CODE_MINAVAIL			0x000

#define PCI_BA_FILL( _ba_addr, _ba_size) \
    {\
        .addr = _ba_addr, \
        .size = _ba_size, \
        .attr = pci_asAttr_e_64BIT | pci_asAttr_e_INBOUND | pci_asAttr_e_CONTIG,\
        .type = pci_asType_e_MEM\
    }

// hash_table
typedef struct _hash_entry hash_entry_t;
struct _hash_entry {
	void 			*data;
	size_t			len;
	hash_entry_t	*next;
};

typedef struct _hash_table {
	hash_entry_t	**buckets;
	uint32_t		nbuckets;
#define USB_HASH_USE_LEN		0x00000001
	uint32_t 		flags;
	/* in case of deinitializing the other resource based on hash entry's information */
	void			(*destructor)(hash_entry_t *entry);
} hash_table_t;

void ht_init(hash_table_t **ht, uint32_t nbuckets, uint32_t flags );
void ht_fini( hash_table_t *ht );
void ht_add( hash_table_t *ht, void *p, size_t len );
hash_entry_t * ht_get( hash_table_t *ht, void *p, size_t len );
void ht_remove( hash_table_t *ht, void *p, size_t len );
void ht_set_destructor(hash_table_t *ht, void (*func)(hash_entry_t *));

int			usb_destroy_thread( pthread_t tid, int chid, int coid );
int			usb_create_thread( int *chid, int *coid, pthread_t *tid, pthread_attr_t *aattr, void *(*func)(void *),
					void *arg, int priority, int *tstate, char *name );

int			usb_set_thread_state( int *tstate, int state );

paddr_t		usb_mphys( const void *addr );
int             usb_pci_dma_addr_translate (pci_devhdl_t hdl, uint64_t addr, uint64_t size, uint64_t* x_addr);

ssize_t		usb_slogf( int opcode, int severity, int verbosity, int vlevel, const char *fmt, ... );

#endif

#if defined(QNXNTO) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/lib/usb/public/sys/usb_support.h $ $Rev: 977663 $")
#endif
