/*
    This CDC ACM driver is an adaptation of GigaDevice's cdc_acm_core driver
    It includes simplified functions for sending/receiving data over USB and
    a better/simpler API. It also uses RISC-V core tick timer instead of 
    Timer 2

    Modified by Fábio Pereira
    www.embeddedsystems.io

    Date: August, 29th, 2020
*/

#include "drv_usb_hw.h"
#include "GD32V_systick.h"
#include "cdc_core.h"
#include <string.h>

#define USBD_VID    0x28e9  /* Vendor ID */
#define USBD_PID    0x018a  /* Product ID */

static uint32_t cdc_cmd = 0xFFU;

uint8_t usb_data_buffer[CDC_ACM_DATA_PACKET_SIZE];
uint8_t usb_cmd_buffer[CDC_ACM_CMD_PACKET_SIZE];

__IO uint8_t packetSent = 1U;
__IO uint32_t receiveLength = 0U;

usb_core_driver thisUSBdev = 
{
    .dev = {
        .desc = {
            .dev_desc       = (uint8_t *)&device_descriptor,
            .config_desc    = (uint8_t *)&configuration_descriptor,
            .strings        = usbd_strings,
        }
    }
};

usb_dev *thisDevice = NULL;

typedef struct
{
    uint32_t dwDTERate;   /* data terminal rate */
    uint8_t  bCharFormat; /* stop bits */
    uint8_t  bParityType; /* parity */
    uint8_t  bDataBits;   /* data bits */
} line_coding_struct;

line_coding_struct linecoding =
{
    115200, /* baud rate     */
    0x00,   /* stop bits - 1 */
    0x00,   /* parity - none */
    0x08    /* num of bits 8 */
};

/* note:it should use the C99 standard when compiling the below codes */
/* USB standard device descriptor */
const usb_desc_dev device_descriptor =
{
    .header = 
     {
         .bLength = USB_DEV_DESC_LEN, 
         .bDescriptorType = USB_DESCTYPE_DEV
     },
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x02,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = USB_FS_EP0_MAX_LEN,
    .idVendor = USBD_VID,
    .idProduct = USBD_PID,
    .bcdDevice = 0x0100,
    .iManufacturer = STR_IDX_MFC,
    .iProduct = STR_IDX_PRODUCT,
    .iSerialNumber = STR_IDX_SERIAL,
    .bNumberConfigurations = USBD_CFG_MAX_NUM
};

/* USB device configuration descriptor */
usb_descriptor_configuration_set_struct configuration_descriptor = 
{
    .config = 
    {
        .header = 
         {
            .bLength = USB_CFG_DESC_LEN,
            .bDescriptorType = USB_DESCTYPE_CONFIG
         },
        .wTotalLength = USB_CDC_ACM_CONFIG_DESC_SIZE,
        .bNumInterfaces = 0x02,
        .bConfigurationValue = 0x01,
        .iConfiguration = 0x00,
        .bmAttributes = 0x80,
        .bMaxPower = 0x32
    },

    .cdc_loopback_interface = 
    {
        .header = 
         {
             .bLength = USB_ITF_DESC_LEN,
             .bDescriptorType = USB_DESCTYPE_ITF 
         },
        .bInterfaceNumber = 0x00,
        .bAlternateSetting = 0x00,
        .bNumEndpoints = 0x01,
        .bInterfaceClass = 0x02,
        .bInterfaceSubClass = 0x02,
        .bInterfaceProtocol = 0x00,
        .iInterface = 0x00
    },

    .cdc_loopback_header = 
    {
        .header =
         {
            .bLength = sizeof(usb_descriptor_header_function_struct), 
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype = 0x00,
        .bcdCDC = 0x0110
    },

    .cdc_loopback_call_managment = 
    {
        .header = 
         {
            .bLength = sizeof(usb_descriptor_call_managment_function_struct), 
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype = 0x01,
        .bmCapabilities = 0x00,
        .bDataInterface = 0x01
    },

    .cdc_loopback_acm = 
    {
        .header = 
         {
            .bLength = sizeof(usb_descriptor_acm_function_struct), 
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype = 0x02,
        .bmCapabilities = 0x02,
    },

    .cdc_loopback_union = 
    {
        .header = 
         {
            .bLength = sizeof(usb_descriptor_union_function_struct), 
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype = 0x06,
        .bMasterInterface = 0x00,
        .bSlaveInterface0 = 0x01,
    },

    .cdc_loopback_cmd_endpoint = 
    {
        .header = 
         {
            .bLength = USB_EP_DESC_LEN, 
            .bDescriptorType = USB_DESCTYPE_EP
         },
        .bEndpointAddress = CDC_ACM_CMD_EP,
        .bmAttributes = 0x03,
        .wMaxPacketSize = CDC_ACM_CMD_PACKET_SIZE,
        .bInterval = 0x0A
    },

    .cdc_loopback_data_interface = 
    {
        .header = 
         {
            .bLength = USB_ITF_DESC_LEN,
            .bDescriptorType = USB_DESCTYPE_ITF
         },
        .bInterfaceNumber = 0x01,
        .bAlternateSetting = 0x00,
        .bNumEndpoints = 0x02,
        .bInterfaceClass = 0x0A,
        .bInterfaceSubClass = 0x00,
        .bInterfaceProtocol = 0x00,
        .iInterface = 0x00
    },

    .cdc_loopback_out_endpoint = 
    {
        .header = 
         {
             .bLength = USB_EP_DESC_LEN, 
             .bDescriptorType = USB_DESCTYPE_EP 
         },
        .bEndpointAddress = CDC_ACM_DATA_OUT_EP,
        .bmAttributes = 0x02,
        .wMaxPacketSize = CDC_ACM_DATA_PACKET_SIZE,
        .bInterval = 0x00
    },

    .cdc_loopback_in_endpoint = 
    {
        .header = 
         {
             .bLength = USB_EP_DESC_LEN, 
             .bDescriptorType = USB_DESCTYPE_EP 
         },
        .bEndpointAddress = CDC_ACM_DATA_IN_EP,
        .bmAttributes = 0x02,
        .wMaxPacketSize = CDC_ACM_DATA_PACKET_SIZE,
        .bInterval = 0x00
    }
};

/* USB language ID Descriptor */
const usb_desc_LANGID usbd_language_id_desc = 
{
    .header = 
     {
         .bLength = sizeof(usb_desc_LANGID), 
         .bDescriptorType = USB_DESCTYPE_STR
     },
    .wLANGID = ENG_LANGID
};

void* const usbd_strings[] = 
{
    [STR_IDX_LANGID] = (uint8_t *)&usbd_language_id_desc,
    [STR_IDX_MFC] = USBD_STRING_DESC("GigaDevice"),
    [STR_IDX_PRODUCT] = USBD_STRING_DESC("GD32 USB CDC ACM in FS Mode"),
    [STR_IDX_SERIAL] = USBD_STRING_DESC("GD32XXX-3.0.0-7z8x9yer")
};

uint8_t cdc_acm_core_init (usb_dev *pudev, uint8_t config_index);
uint8_t cdc_acm_core_deinit (usb_dev *pudev, uint8_t config_index);
uint8_t cdc_acm_req_handler (usb_dev *pudev, usb_req *req);
uint8_t cdc_acm_data_in_handler (usb_dev *pudev, uint8_t ep_id);
uint8_t cdc_acm_data_out_handler (usb_dev *pudev, uint8_t ep_id);
uint8_t cdc_acm_EP0_RxReady (usb_dev *pudev);

usb_class_core usbd_cdc_cb = {
    .command         = NO_CMD,
    .alter_set       = 0,
    .init            = cdc_acm_core_init,
    .deinit          = cdc_acm_core_deinit,
    .req_proc        = cdc_acm_req_handler,
    .data_in         = cdc_acm_data_in_handler,
    .data_out        = cdc_acm_data_out_handler
};

/*!
    \brief      initialize the CDC ACM device
    \param[in]  pudev: pointer to USB device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     USB device operation status
*/
uint8_t cdc_acm_core_init (usb_dev *pudev, uint8_t config_index)
{
    /* initialize the data Tx/Rx endpoint */
    usbd_ep_setup(pudev, &(configuration_descriptor.cdc_loopback_in_endpoint));
    usbd_ep_setup(pudev, &(configuration_descriptor.cdc_loopback_out_endpoint));

    /* initialize the command Tx endpoint */
    usbd_ep_setup(pudev, &(configuration_descriptor.cdc_loopback_cmd_endpoint));

    thisDevice = pudev;

    return USBD_OK;
}

/*!
    \brief      de-initialize the CDC ACM device
    \param[in]  pudev: pointer to USB device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     USB device operation status
*/
uint8_t cdc_acm_core_deinit (usb_dev *pudev, uint8_t config_index)
{
    /* deinitialize the data Tx/Rx endpoint */
    usbd_ep_clear(pudev, CDC_ACM_DATA_IN_EP);
    usbd_ep_clear(pudev, CDC_ACM_DATA_OUT_EP);

    /* deinitialize the command Tx endpoint */
    usbd_ep_clear(pudev, CDC_ACM_CMD_EP);

    return USBD_OK;
}

/*!
    \brief      handle CDC ACM data
    \param[in]  pudev: pointer to USB device instance
    \param[in]  rx_tx: data transfer direction:
      \arg        USBD_TX
      \arg        USBD_RX
    \param[in]  ep_id: endpoint identifier
    \param[out] none
    \retval     USB device operation status
*/
uint8_t  cdc_acm_data_out_handler (usb_dev *pudev, uint8_t ep_id)
{
    if ((EP0_OUT & 0x7F) == ep_id) 
    {
        cdc_acm_EP0_RxReady (pudev);
    } 
    else if ((CDC_ACM_DATA_OUT_EP & 0x7F) == ep_id) 
    {
        receiveLength = usbd_rxcount_get(pudev, CDC_ACM_DATA_OUT_EP);

        return USBD_OK;
    }
    return USBD_FAIL;
}

uint8_t  cdc_acm_data_in_handler (usb_dev *pudev, uint8_t ep_id)
{
    if ((CDC_ACM_DATA_IN_EP & 0x7F) == ep_id) 
    {
        usb_transc *transc = &pudev->dev.transc_in[EP_ID(ep_id)];

        if ((transc->xfer_len % transc->max_len == 0) && (transc->xfer_len != 0)) {
            usbd_ep_send (pudev, ep_id, NULL, 0U);
        } else {
            packetSent = 1;
        }
        return USBD_OK;
    } 
    return USBD_FAIL;
}


/*!
    \brief      handle the CDC ACM class-specific requests
    \param[in]  pudev: pointer to USB device instance
    \param[in]  req: device class-specific request
    \param[out] none
    \retval     USB device operation status
*/
uint8_t cdc_acm_req_handler (usb_dev *pudev, usb_req *req)
{
    switch (req->bRequest) 
    {
        case SEND_ENCAPSULATED_COMMAND:
            break;
        case GET_ENCAPSULATED_RESPONSE:
            break;
        case SET_COMM_FEATURE:
            break;
        case GET_COMM_FEATURE:
            break;
        case CLEAR_COMM_FEATURE:
            break;
        case SET_LINE_CODING:
            /* set the value of the current command to be processed */
            cdc_cmd = req->bRequest;
            /* enable EP0 prepare to receive command data packet */
            pudev->dev.transc_out[0].xfer_buf = usb_cmd_buffer;
            pudev->dev.transc_out[0].remain_len = req->wLength;
            break;
        case GET_LINE_CODING:
            usb_cmd_buffer[0] = (uint8_t)(linecoding.dwDTERate);
            usb_cmd_buffer[1] = (uint8_t)(linecoding.dwDTERate >> 8);
            usb_cmd_buffer[2] = (uint8_t)(linecoding.dwDTERate >> 16);
            usb_cmd_buffer[3] = (uint8_t)(linecoding.dwDTERate >> 24);
            usb_cmd_buffer[4] = linecoding.bCharFormat;
            usb_cmd_buffer[5] = linecoding.bParityType;
            usb_cmd_buffer[6] = linecoding.bDataBits;
            /* send the request data to the host */
            pudev->dev.transc_in[0].xfer_buf = usb_cmd_buffer;
            pudev->dev.transc_in[0].remain_len = req->wLength;
            break;
        case SET_CONTROL_LINE_STATE:
            break;
        case SEND_BREAK:
            break;
        default:
            break;
    }

    return USBD_OK;
}

/*!
    \brief      command data received on control endpoint
    \param[in]  pudev: pointer to USB device instance
    \param[out] none
    \retval     USB device operation status
*/
uint8_t cdc_acm_EP0_RxReady (usb_dev *pudev)
{
    if (NO_CMD != cdc_cmd) {
        /* process the command data */
        linecoding.dwDTERate = (uint32_t)(usb_cmd_buffer[0] | 
                                         (usb_cmd_buffer[1] << 8) |
                                         (usb_cmd_buffer[2] << 16) |
                                         (usb_cmd_buffer[3] << 24));

        linecoding.bCharFormat = usb_cmd_buffer[4];
        linecoding.bParityType = usb_cmd_buffer[5];
        linecoding.bDataBits = usb_cmd_buffer[6];

        cdc_cmd = NO_CMD;
    }

    return USBD_OK;
}

/*
    Initialize the CDC ACM device
*/
void cdc_init(void)
{
    eclic_global_interrupt_enable();	
    eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL2_PRIO2);
    usb_rcu_config();
    usb_intr_config();
    usbd_init (&thisUSBdev, USB_CORE_ENUM_FS, &usbd_cdc_cb);
}

/*
    Process CDC device communication
*/
void cdc_process(void)
{
    if (packetSent) {
        // perform a receive operation to retrieve a new packet
        usbd_ep_recev(thisDevice, CDC_ACM_DATA_OUT_EP, usb_data_buffer, CDC_ACM_DATA_PACKET_SIZE);        
    }
}

/*
    Sends data through CDC connection
    @param buffer - a pointer to an unit8_t buffer with data to send
    @param size - the number of bytes to send
*/
void cdc_sendData(uint8_t * buffer, uint8_t size)
{
    while (!packetSent);    // wait until all packets were sent
    packetSent = 0;
    if (size > CDC_ACM_DATA_PACKET_SIZE) size = CDC_ACM_DATA_PACKET_SIZE;
    memcpy(usb_data_buffer, buffer, size);
    usbd_ep_send(thisDevice, CDC_ACM_DATA_IN_EP, usb_data_buffer, size);
}

/*
    Sends a single character through CDC connection
    @param data - character to send
*/
void cdc_sendChar(char data)
{
    while (!packetSent);    // wait until all packets were sent
    packetSent = 0;
    usb_data_buffer[0] = data;
    usbd_ep_send(thisDevice, CDC_ACM_DATA_IN_EP, usb_data_buffer, 1);
}

/*
    Checks and retrieve new received data
    @param buffer - a pointer to an unit8_t buffer to store received data
    @param maxSize - maximum number of bytes to receive
    @return the number of received bytes stored in the buffer
*/
uint32_t cdc_getReceivedData(uint8_t * buffer, uint32_t maxSize)
{
    uint32_t result = 0;
    uint32_t tempSize = receiveLength;
    if (tempSize) {
        // there is new data in the receive buffer
        // we will limit the number or received characters to maxSize
        if (receiveLength > maxSize) tempSize = maxSize;
        memcpy(buffer, usb_data_buffer, tempSize);
        result = tempSize;
        receiveLength -= tempSize;
    }   
    return result;
}

/*
    Checks whether CDC device is configured or not
    Return TRUE if configured
*/
bool cdc_isConfigured(void)
{
    return (USBD_CONFIGURED == thisUSBdev.dev.cur_status);
}

/*
    Prints a text string through CDC connection
    @param string - a null terminated char string
*/
void cdc_print(char *string)
{
    cdc_sendData((uint8_t *)string, (uint8_t)strlen(string));
}