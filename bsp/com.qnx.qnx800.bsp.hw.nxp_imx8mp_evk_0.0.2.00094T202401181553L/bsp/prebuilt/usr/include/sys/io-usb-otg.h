/*
 * $QNXLicenseC:
 * Copyright 2006,2019 QNX Software Systems. All Rights Reserved.
 *
 * This source code may contain confidential information of QNX Software
 * Systems (QSS) and its licensors.  Any use, reproduction, modification,
 * disclosure, distribution or transfer of this software, or any software
 * that includes or is based upon any of this code, is prohibited unless
 * expressly authorized by QSS by written agreement.  For more information
 * (including whether this source code file has been published) please
 * email licensing@qnx.com. $
*/

/*
 *  sys/io-usbotg.h
 *
 */

#ifndef __NTO_IO_USBOTG_H_INCLUDED
#define __NTO_IO_USBOTG_H_INCLUDED


#include <inttypes.h>

#include <sys/dispatch.h>
#include <sys/types.h>

#include <sys/usb100.h>
#include <sys/usbdi.h>

#include <sys/usb_smmu.h>
#include <pci/pci.h>
#include <pci/pci_ccode.h>


__BEGIN_DECLS

#define XSTR(x)						#x
#define IOUSB_STRINGIZE(x)			XSTR(x)

#define IOUSB_4G_ADDR_MSK	0xffffffff00000000LL

#if ( __PTR_BITS__ == 64 )
#define IOUSB_PULSE_PTR( __coid, __priority, __code, __value )	MsgSendPulsePtr( __coid, __priority, __code, (void *)__value )
#define IOUSB_PULSE_PTR_R( __coid, __priority, __code, __value )	MsgSendPulsePtr_r( __coid, __priority, __code, (void *)__value )
#else
#define IOUSB_PULSE_PTR( __coid, __priority, __code, __value )	MsgSendPulse( __coid, __priority, __code, (int)__value )
#define IOUSB_PULSE_PTR_R( __coid, __priority, __code, __value )	MsgSendPulse_r( __coid, __priority, __code, (int)__value )
#endif

/* The following version numbers control the interface 
 * presented to the devu by the server.  This includes
 * iousb_self_t and referenced data structures. */
#define IOUSB_HCD_VERSION			0x0002
#define IOUSB_DCD_VERSION			0x0001

/* The following version numbers control the interface 
 * presented to the server by the devu library.  This 
 * includes io_usb_otg_dll_entry_t and referenced data 
 * stuctures. */
#define DEVU_HCD_VERSION			0x1
#define DEVU_DCD_VERSION			0x0


/* Controller states */
#define USB_CONTROLLER_ACTIVE			1
#define USB_CONTROLLER_INACTIVE			0

typedef struct _io_usb_otg_dll_entry	io_usb_otg_dll_entry_t;
typedef struct _usb_hcd 				usb_hcd_t;
typedef struct _usb_dcd					usb_dcd_t;
typedef struct _usb_rhub_device 		usb_rhub_device_t;
typedef struct _vhd_event				vhd_event_t;
typedef struct _vhd_methods				vhd_methods_t;
struct _ep0_request_handler;


typedef struct _iousb_endpoint_t  {
	usbd_endpoint_descriptor_t 	edesc;		/* USB defined endpoint descriptor information */
	uint32_t					flags;		/* Endpoint flags set by HW driver */
	void 						*user;		/* used by HW driver */
	uint32_t					nstreams;
} iousb_endpoint_t;

#define IOUSB_ENDPOINT_FLAGS_BMAP		0x10
#define IOUSB_ENDPOINT_FLAGS_NO_STATUS	0x20  /* Try to short circuit status stage */
#define IOUSB_ENDPOINT_MURB				0x40  /* this endpoint can take multiple urb */

typedef struct _iousb_device {
	uint32_t		device_address;
	uint32_t		device_port;
	uint32_t		device_speed;

#define IOUSB_DEVICE_FLAGS_HUB			0x1
#define IOUSB_DEVICE_FLAGS_MULTI_TT		0x2
	uint32_t		flags;
	uint32_t		nports;
} iousb_device_t;


typedef struct _iousb_transfer {
	uint32_t	status;
	uint32_t	flags;
	uint32_t	usbdi_flags;
	uint32_t	timeout;
	uint32_t	isoch_frame;
	uint8_t		*buffer;
	paddr64_t	buffer_paddr;
	uint32_t	buffer_len;
	uint32_t	actual_len;
	void		(*urb_complete_cbf)  ( iousb_endpoint_t *endp, struct _iousb_transfer *urb, uint32_t status, uint32_t tlen );
	void		*xdata_ptr;
	uint32_t	stream_id;
} iousb_transfer_t;


/*
 * Calls HC drivers can make into io-usb
 */
typedef struct _usb_hcd_self {
	uint32_t			version_nfuncs; 				/* Upper 16 bits are the version, lower 16 are nfuncs.
														   Macros for determining it's value and reading it are
														   provided below. */
	int 				(*hc_bus_state)					( usb_hcd_t *uhcd, uint32_t bus_state, uint32_t flags );
	int 				(*hc_port_status_change) 		( usb_hcd_t *uhcd, _uint32 change_mask );
	int 				(*hc_port_resume_signalling) 	( usb_hcd_t *uhcd, uint32_t portno );
	iousb_device_t		*(*iousb_get_parent_device) 	( iousb_device_t *device );
	iousb_endpoint_t	*(*iousb_get_endpoint)			( iousb_device_t *device, int epn );
	int 				(*hc_reserved) 					( usb_hcd_t *uhcd, void *data );

	int					(*usb_smmu_init)				( usb_smmu_t *smmu, uint64_t addr, uint64_t size );
	int					(*usb_smmu_fini)				( usb_smmu_t *smmu, uint64_t addr, uint64_t size );
	int					(*usb_smmu_map)					( usb_smmu_t *smmu, uint64_t addr, uint64_t size );
	int					(*usb_smmu_unmap)				( usb_smmu_t *smmu, uint64_t addr, uint64_t size );
	int					(*usb_smmu_get_flags)			( void );
	int					(*usb_smmu_add_mmio)			( usb_smmu_t *smmu, uint64_t addr, uint64_t size, int map );
} usb_hcd_self_t;

#define _IO_USB_NFUNCS			((sizeof( usb_hcd_self_t ) - sizeof( u_int) ) / sizeof( void * ))
#define _IO_USB_VER_NFUNCS		(( IOUSB_HCD_VERSION << 16 ) | _IO_USB_NFUNCS )
#define _IO_USB_VERSION(_self)	(( (_self)->self.hcd->version_nfuncs >> 16 ) & 0xFFFF)


/*
 * Calls DC drivers can make into io-usb
 */
typedef struct _usb_dcd_self {
		
	uint32_t		version_nfuncs; 				/* Upper 16 bits are the version, lower 16 are nfuncs.
													   Macros for determining it's value and reading it are
													   provided below. */
	int 			(*usbdc_device_state_change) 	( usb_dcd_t *udcd, uint32_t device_state );
	int 			(*usbdc_endpoint_state_change)	( iousb_endpoint_t *endp, uint32_t state );
	int 			(*usbdc_setup_packet_process) 	( usb_dcd_t *udcd, uint8_t *buff );
	void 			(*usbdc_set_device_speed) 		( usb_dcd_t *udcd, uint32_t speed );
	int 			(*usbdc_reserved) 				( usb_dcd_t *udcd, void *data );

	int				(*usbdc_vhd_init)				( usb_dcd_t *udcd, char *vhd_name, void *client_hdl, int (*vhd_event)( void *vhd_driver_hdl, vhd_event_t *vev ) );
	int				(*usbdc_vhd_detach)				( usb_dcd_t *udcd, void *vhd_attach_hdl );
	int				(*usbdc_vhd_attach)				( usb_dcd_t *udcd, void **r_vhd_attach_hdl, char *vhd_name, int vhd_idx, void **r_vhd_driver_hdl,
														vhd_methods_t **r_vhd_m );

	int				(*usb_smmu_init)				( usb_smmu_t *smmu, uint64_t addr, uint64_t size );
	int				(*usb_smmu_fini)				( usb_smmu_t *smmu, uint64_t addr, uint64_t size );
	int				(*usb_smmu_map)					( usb_smmu_t *smmu, uint64_t addr, uint64_t size );
	int				(*usb_smmu_unmap)				( usb_smmu_t *smmu, uint64_t addr, uint64_t size );
	int				(*usb_smmu_get_flags)			( void );
	int				(*usb_smmu_add_mmio)			( usb_smmu_t *smmu, uint64_t addr, uint64_t size, int map );
} usb_dcd_self_t;

#define _IO_USBDC_NFUNCS			((sizeof(usb_dcd_self_t)-sizeof(u_int))/sizeof(void *))
#define _IO_USBDC_VER_NFUNCS		(( IOUSB_DCD_VERSION << 16 ) | _IO_USBDC_NFUNCS )
#define _IO_USBDC_VERSION(_self)	(( (_self)->self.dcd->version_nfuncs >> 16 ) & 0xFFFF)

#if 0

typedef struct _usb_rhub_self {
	uint32_t		nfuncs;
	int 			(*usb_rhub_reserved) 				( struct _usb_rhub_device *rhubd, void *data );
} usb_rhub_self_t;

#endif

typedef struct _iousb_self {
	union {
		usb_hcd_self_t		*hcd;
		usb_dcd_self_t		*dcd;
//		usb_rhub_self_t		*rhub;
	} self;
} iousb_self_t;


#define IOUSB_BUS_STATE_FLAG_NOTIFY_ASYNC			0x01


typedef struct _iousb_pipe_methods {
	int			(*iousb_endpoint_enable)	( void *chdl, iousb_device_t *device, iousb_endpoint_t *endp );
	int			(*iousb_endpoint_disable)	( void *chdl, iousb_endpoint_t *endp );
	int			(*iousb_transfer_data)		( void *chdl, iousb_transfer_t *urb, iousb_endpoint_t *endp, _uint8 *buffer, uint32_t length, uint32_t flags );
	int			(*iousb_transfer_abort)		( void *chdl, iousb_transfer_t *urb, iousb_endpoint_t *endp );
	void		(*reserved)					( void *chdl, void *data );
} iousb_pipe_methods_t;

#define _USB_HC_PIPE_METHODS ((sizeof(hc_pipe_methods_t) - sizeof(uint32_t)) / sizeof(void *))


/* USB transfer flags */
#define  PIPE_FLAGS_TOKEN_SETUP		0x01
#define  PIPE_FLAGS_TOKEN_STATUS	0x02
#define  PIPE_FLAGS_TOKEN_IN		0x04
#define  PIPE_FLAGS_TOKEN_OUT		0x08
#define	 PIPE_FLAGS_BUFFER_PHYS 	0x10
#define  PIPE_FLAGS_MULTI_XFER		0x20
#define  PIPE_FLAGS_LAST_PACKET 	0x80000000

typedef struct _iousb_ep_pipe_methods {
	iousb_pipe_methods_t 	*ctrl_pipe_methods;
	iousb_pipe_methods_t 	*isoch_pipe_methods;
	iousb_pipe_methods_t 	*bulk_pipe_methods;
	iousb_pipe_methods_t	*int_pipe_methods;
} iousb_ep_pipe_methods_t;

/*
 * Entry point into HC Drivers.
 */
typedef struct _usb_hcd_methods {
	int						nentries;
	int						(*hc_init)					( usb_hcd_t *uhcd, uint32_t flags, char *args );
	int						(*hc_start)					( usb_hcd_t *uhcd );
	int						(*hc_stop)					( usb_hcd_t *uhcd );
	int						(*hc_shutdown)				( usb_hcd_t *uhcd );
	int						(*hc_set_bus_state)         ( usb_hcd_t *uhcd, uint32_t bus_state );
	void					*(*hc_dma_memory_malloc)	( usb_hcd_t *uhcd, size_t len );
	int						(*hc_dma_memory_free) 		( usb_hcd_t *uhcd, void *addr, size_t len );
	int						(*hc_interrupt)				( usb_hcd_t *uhcd );
	int						(*hc_set_port_feature)      ( usb_hcd_t *uhcd, uint32_t port, uint32_t feature );
	int						(*hc_clear_port_feature)    ( usb_hcd_t *uhcd, uint32_t port, uint32_t feature );
	int						(*hc_check_port_status)		( usb_hcd_t *uhcd, uint32_t *portno_status );
	int						(*hc_check_device_connected)( usb_hcd_t *uhcd, uint32_t port );
	int						(*hc_get_root_device_speed)	( usb_hcd_t *uhcd, uint32_t port );
	int						(*hc_get_current_frame)		( usb_hcd_t *uhcd );

	/* pipe handling methods 			*/
	iousb_ep_pipe_methods_t	*ep_pipe_methods;
} usb_hcd_methods_t;


#define _USB_HC_METHODS ((sizeof(usb_hcd_methods_t) - sizeof(uint32_t)) / sizeof(void *))

struct _vhd_methods {
	int						nentries;
	iousb_ep_pipe_methods_t	*ep_pipe_methods;
};

struct _vhd_event {
#define VHD_EVT_INSERTION			0x01
#define VHD_EVT_REMOVAL				0x02
#define VHD_EVT_SUSPEND				0x03
#define VHD_EVT_RESUME				0x04
#define VHD_EVT_RESET				0x05
#define VHD_EVT_SETUP_PKT			0x06
#define VHD_EVT_PNP_CONTROLLER_INSERTED		0x07
#define VHD_EVT_PNP_CONTROLLER_REMOVED		0x08
	uint32_t 				ev_type;
	uint32_t				ev_len;
	void					*ev_data;
};

/*
 * Entry points into DC Drivers.
 */
typedef struct _usb_dcd_methods {
	int						nentries;
	int						(*dc_init)					( usb_dcd_t *udcd, usb_dcd_self_t *udc_self, char *args );
	int						(*dc_start)					( usb_dcd_t *udcd );
	int						(*dc_stop)					( usb_dcd_t *udcd );
	int						(*dc_shutdown)				( usb_dcd_t *udcd );

	void					*(*dc_dma_memory_malloc)	( usb_dcd_t *udcd, size_t len );
	int						(*dc_dma_memory_free) 		( usb_dcd_t *udcd, uint32_t *addr, size_t len );

	int						(*dc_set_bus_state) 		( usb_dcd_t *udcd, uint32_t bus_state, uint32_t aux );
	int						(*dc_set_device_feature) 	( usb_dcd_t *udcd, uint32_t feature, _uint16 index );
	int						(*dc_clear_device_feature) 	( usb_dcd_t *udcd, uint32_t feature );

	int						(*dc_set_device_address) 	( usb_dcd_t *udcd, uint32_t address );

	int						(*dc_get_descriptor)		( usb_dcd_t *udcd, uint8_t type, uint8_t index, uint16_t lang_id, uint8_t **desc, uint32_t speed );

	int						(*dc_select_configuration)	( usb_dcd_t *udcd, uint8_t config );
	int						(*dc_interrupt)				( usb_dcd_t *udcd );

	int						(*dc_set_endpoint_state) 	( usb_dcd_t *udcd, iousb_endpoint_t *ep, uint32_t ep_state );
	int						(*dc_clear_endpoint_state)	( usb_dcd_t *udcd, iousb_endpoint_t *ep, uint32_t ep_state );

	/* pipe handling methods 			*/
	iousb_ep_pipe_methods_t	*ep_pipe_methods;
} usb_dcd_methods_t;


#define _USB_DC_METHODS ((sizeof(dc_methods_t) - sizeof(uint32_t)) / sizeof(void *))

#if 0

/*
 * Root hub device entry points (Root Hub Driver)
 */
typedef struct _usb_rhub_ep0_methods {
	struct _ep0_request_handler		*standard_handlers;		// NULL means use defaults
	struct _ep0_request_handler		*class_handlers;		// NULL means use defaults
	struct _ep0_request_handler		*vendor_handlers;		// NULL means use defaults
	struct _ep0_request_handler		*override_handlers;		// allows to override any ep0 request
} usb_rhub_ep0_methods_t;


typedef struct _rhub_device_methods {
	usb_rhub_ep0_methods_t		*ep0_methods;					// provide methods to override default_
	iousb_pipe_methods_t 		*pipe_methods_ep0;
	iousb_pipe_methods_t 		*pipe_methods_notification;
} rhub_device_methods_t;


typedef struct _usb_rhub_ctrl {
	void					*rhub_ctx;
	struct _iousb_device	*iousb_device;
	struct _iousb_transfer	*urb_ep0;				// pointer to store ep0 transfer from the stack
	struct _iousb_transfer	*urb_notification;		// pointer to store notification transfer from the stack.
	void					*user;
} usb_rhub_ctrl_t;


typedef struct _usb_rhub_methods {
	uint32_t 				nentries;
	int 					(*rhub_init)			( usb_rhub_ctrl_t *rhub_ctrl, usb_rhub_self_t *rhub_self, char *args );
	int						(*rhub_destroy)			( usb_rhub_ctrl_t *rhub_ctrl );
	int         			(*rhub_start)			( usb_rhub_ctrl_t *rhub_ctrl );
	int         			(*rhub_stop)			( usb_rhub_ctrl_t *rhub_ctrl );
	int         			(*rhub_state)			( usb_rhub_ctrl_t *rhub_ctrl, uint32_t state );
	rhub_device_methods_t 	*rhub_device_methods;
} usb_rhub_methods_t;
#endif


#if 0
struct _usb_rhub_device {
	iousb_device_t      	*iousb_device;
	iousb_transfer_t		*urb_ep0;						// store transfers on rhub ep0 endpoint
	iousb_transfer_t		*urb_notification;				// store transfers on rhub notification endpoint 
	struct _usb_rhub_ctrl	*rhub_ctrl;
	void					*user;
};
#endif

/*
 * Entry points into OTG Drivers.
 */
typedef struct _usb_otg_methods {


} usb_otg_methods_t;

#define USBDC_DEVICE_STATE_CONNECTED		0x01
#define USBDC_DEVICE_STATE_DISCONNECTED		0x02


typedef struct _usb_controller_methods {
		usb_hcd_methods_t	*hc_methods;
		usb_dcd_methods_t	*dc_methods;
//		usb_rhub_methods_t	*rhub_methods;
		usb_otg_methods_t	*otg_methods;
		void				*reserved;
} usb_controller_methods;


/*
 * Entry points for DLLs
 */

struct _io_usb_otg_dll_entry {
	char					*name;										/* Name of DLL						*/
	uint32_t				version;
	uint32_t				rsvd;
	uint32_t				device_interest;				 			/* Chip type this DLL can handle	*/
	uint32_t				vid;										/* change to allow full spec by DLL( PCI structure )*/
	uint32_t				did;
	uint32_t				ctrl_type;

	int 					(*init) 	( void *dll_hdl, dispatch_t *dpp, iousb_self_t *iousb_self, char *options );
	int 					(*shutdown)	( void *dll_hdl );
	usb_controller_methods	*usbctrl_methods;					/*  Controller methods 				*/
};

#define USB_CONTROLLER_HOST			0x01
#define USB_CONTROLLER_DEVICE		0x02
#define USB_CONTROLLER_OTG			0x04
#define USB_CONTROLLER_RHUB			0x08


/*
 * Common HW control
 */
#define ROUND_UP(x, a) ((typeof(x))(((x) + ((typeof(x))(a)-1)) & ~((typeof(x))(a)-1)))
#define MSIX_ALIGN 4096

typedef struct _usb_pci_dev_info {
	pci_vid_t VendorId;
	pci_did_t DeviceId;
	pci_ssvid_t SubsystemVendorId;
	pci_irq_t Irq;
	pci_revid_t RevId;
	uint64_t CpuBmstrTranslation;
	pci_ba_t ba;
	pci_bdf_t bdf;
	pci_cap_t pcie;
	pci_cap_t msix;
	pci_cap_t msi;

} usb_pci_dev_info_t;

typedef struct _iousb_hw_ctrl {
    char 	         			*cname;                 /* Descriptive name   		    */
	usb_pci_dev_info_t			*pci_inf;
	uint32_t					cindex;
	uint32_t					rsvd;
	uint64_t					capabilities;			/* capabilites of controller	*/
	uint32_t					max_transfer_size;      /* Size of the max transfer     */
	uint32_t					max_unaligned_xfer;		/* Size of the max transfer with unaligned buffer */
	uint64_t					buff_alignment_mask;	/* mask fo buffer alignment    */
	uint8_t						max_streams;		/* Number or streams supported is defined as 2^(max_streams+1)
										   for max_streams > 0.  This reflects the MaxPSASize in XHCI 5.3.6 */
} iousb_hw_ctrl_t;


/*
 * Structure passed to HC Drivers
 */
struct _usb_hcd {
	iousb_hw_ctrl_t				hw_ctrl;
	usb_hcd_self_t				*hcd_self;
	void						*pdev;
	uint32_t					num_root_hubs;
	uint32_t					root_hub_status;		/* port change bitmap			*/
	uint32_t         			AvailableBandwidth;     /* Guaranteed bandwidth         */
	uint32_t					ctrl_retry;				/* ctrl retry count 			*/
	uint32_t         			HFrameumber;            /* High 16bit word for frame nb */
	void						*dll_hdl;
	void 						*hc_data;				/* data associated with a HC	*/
};


/* USB HC PCI class codes */
#define USB_CLASS_OHCI          		0x0c0310
#define USB_CLASS_UHCI          		0x0c0300
#define USB_CLASS_EHCI          		0x0c0320
#define USB_CLASS_XHCI          		0x0c0330
#define USB_CLASS_ISP1161       		0x0c6100

/* DLL init flags */
#define IOUSB_USBMGR_ENABLED			0x80000000

/* HC capability flags */
#define HC_CAP_INTERRUPT	    		0x00000001	/* #define USBD_HCD_CAP_CNTL				0x0001 */
#define HC_CAP_BULK		    			0x00000002	/* #define USBD_HCD_CAP_INTR				0x0002 */
#define HC_CAP_ISOCH		    		0x00000004	/* #define USBD_HCD_CAP_ISOCH				0x0008 */
#define HC_CAP_REMOTE_WAKEUP_CAPABLE	0x00000100	/* #define USBD_HCD_CAP_REMOTE_WAKEUP		0x0100 */

#define HC_CAP_AYSNC_PARK_MODE			(0x0000001LL << 32)
#define HC_CAP_64_BIT_ADDRESSING    	(0x0000002LL << 32)
#define HC_CAP_IND_PORT_STATUS    		(0x0000004LL << 32)

#define HC_CAP_PORT_CHANGE_REPORT		(0x0000008LL << 32)
#define HC_CAP_MAP_CLIENT_BUFF			(0x4000000LL << 32)
#define HC_CAP_DMA_MEMORY_SPECIAL		(0x8000000LL << 32)

/* DC capability flags 	*/
#define DC_CAP_ENDP_STALL_AUTO_CLEARED	0x00000001
#define DC_CAP_LOW_SPEED				0x00000002
#define DC_CAP_FULL_SPEED				0x00000004
#define DC_CAP_HIGH_SPEED				0x00000008
#define DC_CAP_TEST_MODES_SUPPORTED		0x00000010
#define DC_CAP_SUPER_SPEED				0x00000020

/*FIXME: this is temporarily for compatibility, need to be cleanedup */
#define USBD_DCD_CAP_ENDP_STALL_AUTO_CLEARED	0x00000001
#define USBD_DCD_CAP_LOW_SPEED					0x00000002
#define USBD_DCD_CAP_FULL_SPEED					0x00000004
#define USBD_DCD_CAP_HIGH_SPEED					0x00000008
#define USBD_DCD_CAP_TEST_MODES_SUPPORTED		0x00000010
#define USBD_DCD_CAP_SUPER_SPEED				0x00000020

/* bus states */
#define USB_BUS_STATE_RESET				1
#define USB_BUS_STATE_SUSPENDED			2
#define USB_BUS_STATE_RESUME			3
#define USB_BUS_STATE_OPERATIONAL		4

/* chip state */
#define USB_BUS_STATE_START				5
#define USB_BUS_STATE_STOP				6

#define USB_PORT_IOUSB_PHY_RESET		100

/* io-usb specific port features */

#define USB_IOUSB_INTERNAL_FEATURE_CMD_MASK				(0x00ff<<16)
#define USB_IOUSB_INTERNAL_FEATURE_CMD_WK_ENABLE		(0x001<<16)
#define USB_IOUSB_INTERNAL_FEATURE_CMD_PHY_RESET		(0x002<<16)

#define USB_IOUSB_PORT_PHY_RESET				(0x8000<<16)

#define USB_IOUSB_PORT_WK_ENABLE_MASK			(0xff00<<16)
	#define USB_IOUSB_PORT_WAKEUP_CONNECT		(0x0100<<16)
	#define USB_IOUSB_PORT_WAKEUP_DISCONNECT	(0x0200<<16)
	#define USB_IOUSB_PORT_WAKEUP_OVER_CURRENT	(0x0400<<16)

/* define for hub port status */
#define HUB_STATUS_PORT_CONNECTION          0x0001
#define HUB_STATUS_PORT_ENABLE              0x0002
#define HUB_STATUS_PORT_SUSPEND             0x0004
#define HUB_STATUS_PORT_OVER_CURRENT        0x0008
#define HUB_STATUS_PORT_RESET               0x0010
#define HUB_STATUS_PORT_POWER               0x0100
#define HUB_STATUS_PORT_LOW_SPEED           0x0200
#define HUB_STATUS_PORT_HIGH_SPEED          0x0400
#define HUB_STATUS_PORT_TEST_MODE           0x0800
#define HUB_STATUS_PORT_INDICATOR_CONTROL   0x1000


#define HUB_CHANGE_PORT_CONNECTION          0x010000
#define HUB_CHANGE_PORT_ENABLE              0x020000
#define HUB_CHANGE_PORT_SUSPEND             0x040000
#define HUB_CHANGE_PORT_OVER_CURRENT        0x080000
#define HUB_CHANGE_PORT_RESET               0x100000

#define USB_PORT_CLEAR_TT_BUFFER		(1<<16)

// defines for wValue USB_HUB_CLEAR_TT_BUFFER request
#define USB_HUB_CLEAR_TT_S_ENDPOINT_NUMBER 		0
#define USB_HUB_CLEAR_TT_S_DEVICE_ADDRESS		4
#define USB_HUB_CLEAR_TT_S_ENDPOINT_TYPE 		11
#define USB_HUB_CLEAR_TT_M_ENDPOINT_DIRECTION 	0x8000

/* Descriptor convienence macros */
#define IOUSB_ENDPOINT_TYPE( usbep ) 		USB_ENDPOINT_TYPE( (usbep)->edesc.bmAttributes )
#define IOUSB_ENDPOINT_ADDRESS( iousbep ) 	((iousbep)->edesc.bEndpointAddress)
#define IOUSB_ENDPOINT_NUMBER( iousbep ) 	USB_ENDPOINT_NUMBER( (iousbep)->edesc.bEndpointAddress )
#define IOUSB_ENDPOINT_DIRECTION( iousbep ) USB_ENDPOINT_DIRECTION( (iousbep)->edesc.bEndpointAddress )



// events to usbmgr for interface suspend/resume requests
#define IOUSB_EVENT_BUS_OVERCURRENT				10
#define IOUSB_EVENT_DRIVER_INTERFACE_SUSPEND	100
#define IOUSB_EVENT_DRIVER_INTERFACE_RESUME		101

#define IOUSB_EVENT_DEVICE_RESET				50
#define IOUSB_EVENT_DEVICE_RESET_COMPLETE		51
#define IOUSB_EVENT_INTERFACE_RESET			52
#define IOUSB_EVENT_INTERFACE_RESET_COMPLETE	53
#define IOUSB_EVENT_BAD_DEVICE_INSERT			54
#define IOUSB_EVENT_BAD_DEVICE_REMOVE			55

/* Events for Function Driver */

#define IOUSB_EVENT_DEVICE_FUNCTION				80
#define IOUSB_EVENT_VENDOR_COMMAND              1
#define IOUSB_EVENT_DEVICE_STATE_CHANGE 		2
#define IOUSB_EVENT_ENDPOINT_STATE_CHANGE 		3

/*
 * Structure passed to DC Drivers
 */
struct _usb_dcd {
	iousb_hw_ctrl_t				hw_ctrl;
	usb_dcd_self_t				*dcd_self;
	uint32_t					priority;
	uint32_t					verbosity;
	char 						*serial_string;

	uint8_t						selected_config;
	uint8_t						rsrvd[3];

	uint8_t						*setup_buf;
	void						*drvr_hdl;
    void                        *dc_data;               /* data associated with a DC    */
	void						*dll_hdl;
};


/* device speed */
#define	IOUSB_DEVICE_FULL_SPEED		0
#define	IOUSB_DEVICE_LOW_SPEED		1
#define	IOUSB_DEVICE_HIGH_SPEED		2
#define	IOUSB_DEVICE_SUPER_SPEED	3
#define	IOUSB_DEVICE_SUPER_SPEED_PLUS	4
#define	IOUSB_DEVICE_MASK_SPEED		7

/* defines for DC BUS state */
#define IOUSB_BUS_STATE_DISCONNECTED 	1
#define IOUSB_BUS_STATE_CONNECTED 		2
#define IOUSB_BUS_STATE_RESUME 			3


/* define device states */
#define IOUSB_DEVICE_STATE_UNKNOWN                  0
#define IOUSB_DEVICE_STATE_INSERTED                 1
#define IOUSB_DEVICE_STATE_REMOVED                  2
#define IOUSB_DEVICE_STATE_SUSPENDED                3
#define IOUSB_DEVICE_STATE_RESET                    4
#define IOUSB_DEVICE_STATE_RESUMING                 5
#define IOUSB_DEVICE_STATE_UNCONFIGURED             6
#define IOUSB_DEVICE_STATE_CONFIGURED               7
#define IOUSB_DEVICE_STATE_STALLED                  8
#define IOUSB_DEVICE_SUSPEND_REQUEST                9
#define IOUSB_DEVICE_STATE_RESUMED                 10
#define IOUSB_DEVICE_STATE_SHUTDOWN                11
#define IOUSB_DEVICE_STATE_TEST_MODE               12
#define IOUSB_DEVICE_STATE_WALLCHARGER_INSERTED    13
#define IOUSB_DEVICE_STATE_WALLCHARGER_REMOVED     14



#define IOUSB_DEVICE_STATE_READY        0x80000000  // device endpoints are configured and not in error state */

/* define interface states */
#define IOUSB_INTERFACE_STATE_ENABLED    		1
#define IOUSB_INTERFACE_STATE_DISABLED   		2
#define IOUSB_INTERFACE_STATE_ENABLED_COMPLETE 	3
#define IOUSB_INTERFACE_STATE_DISABLED_COMPLETE	4

/* define endpoint states */
#define IOUSB_ENDPOINT_STATE_ENABLE     1
#define IOUSB_ENDPOINT_STATE_DISABLED   2
#define IOUSB_ENDPOINT_STATE_RESET      3
#define IOUSB_ENDPOINT_STATE_READY      4
#define IOUSB_ENDPOINT_STATE_NAK        5
#define IOUSB_ENDPOINT_STATE_STALLED    6

#define WAIT_ROOT_DEVICE_INSERT			50
#define WAIT_ROOT_HUB_RESET 			150



/* DLL debug support */
typedef struct _iousb_dll_debug_entry {
	int		(*debug_init) 		( void *ctrl );
	int		(*debug_command) 	( void *ctrl, uint32_t dcmd, uint32_t level );
	int		(*debug_destroy) 	( void *ctrl );
} iousb_dll_debug_entry_t;


/* internal DEBUG commands */

#define USB_COMPONENT_DEBUG_STACK					1
	#define USB_DEBUG_SET_VERBOSITY						(1<<16)

#define USB_COMPONENT_DEBUG_HW_DLL					2

	#define USB_BUS_DEBUG_STATE_TRIGGER					(1<<16)
	#define USB_BUS_DEBUG_REGISTER_DUMP					(2<<16)
		#define USB_BUS_DEBUG_REGISTER_DUMP_CONTROLLER		1
		#define USB_BUS_DEBUG_REGISTER_DUMP_PHY				2
		#define USB_BUS_DEBUG_REGISTER_DUMP_INTERNAL		4
	#define USB_BUS_DEBUG_SCHEDULE_DUMP					(3<<16)
		#define USB_SCHEDULE_DUMP_INTERNAL					1
		#define USB_SCHEDULE_DUMP_CONTROLLER				2
	#define USB_BUS_DEBUG_PORT							(7<<16)
	#define USB_BUS_DEBUG_XFER_ERROR					(8<<16)
	#define USB_BUS_DEBUG_SET_VERBOSITY					(9<<16)
	#define USB_BUS_DEBUG_PHY_RESET						(10<<16)
	#define USB_BUS_DEBUG_SET_CLOCKS					(11<<16)
		#define USB_BUS_DEBUG_CLOCK_CONTROLLER				1		// bitmask of clock state 1==on, 0==off
		#define USB_BUS_DEBUG_CLOCK_PHY						2
		#define USB_BUS_DEBUG_CLOCK_OTHER					4
	#define USB_BUS_DEBUG_ULPI_REG_READ					(12<<16)
	#define USB_BUS_DEBUG_ULPI_REG_WRITE				(13<<16)
	#define USB_BUS_DEBUG_PORT_SUSPEND					(14<<16)
	#define USB_BUS_DEBUG_PORT_RESUME					(15<<16)
	#define USB_BUS_DEBUG_SET_DEBUG_VERBOSITY			(16<<16)
	#define USB_BUS_DEBUG_ULPI_SET_STP					(17<<16)


__END_DECLS

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/services/usb/stack/core/public/sys/io-usb-otg.h $ $Rev: 964229 $")
#endif
