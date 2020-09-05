/*
    This CDC ACM driver is an adaptation of GigaDevice's cdc_acm_core driver
    It includes simplified functions for sending/receiving data over USB and
    a better/simpler API. It also uses RISC-V core tick timer instead of 
    Timer 2

    Modified by Fábio Pereira
    www.embeddedsystems.io

    Date: August, 29th, 2020
*/

#ifndef CDC_CORE_H
#define CDC_CORE_H

#include "usbd_enum.h"
#include "usb_ch9_std.h"
#include "usbd_transc.h"


#define USB_DESCTYPE_CS_INTERFACE               0x24
#define USB_CDC_ACM_CONFIG_DESC_SIZE            0x43

#define CDC_ACM_DESC_SIZE                       0x3A

#define CDC_ACM_DESC_TYPE                       0x21

#define SEND_ENCAPSULATED_COMMAND               0x00
#define GET_ENCAPSULATED_RESPONSE               0x01
#define SET_COMM_FEATURE                        0x02
#define GET_COMM_FEATURE                        0x03
#define CLEAR_COMM_FEATURE                      0x04
#define SET_LINE_CODING                         0x20
#define GET_LINE_CODING                         0x21
#define SET_CONTROL_LINE_STATE                  0x22
#define SEND_BREAK                              0x23
#define NO_CMD                                  0xFF

#pragma pack(1)

typedef struct
{
    usb_desc_header header;  /*!< descriptor header, including type and size. */
    uint8_t  bDescriptorSubtype;          /*!< bDescriptorSubtype: header function descriptor */
    uint16_t  bcdCDC;                     /*!< bcdCDC: low byte of spec release number (CDC1.10) */
} usb_descriptor_header_function_struct;

typedef struct
{
    usb_desc_header header;  /*!< descriptor header, including type and size. */
    uint8_t  bDescriptorSubtype;          /*!< bDescriptorSubtype:  call management function descriptor */
    uint8_t  bmCapabilities;              /*!< bmCapabilities: D0 is reset, D1 is ignored */
    uint8_t  bDataInterface;              /*!< bDataInterface: 1 interface used for call management */
} usb_descriptor_call_managment_function_struct;

typedef struct
{
    usb_desc_header header;  /*!< descriptor header, including type and size. */
    uint8_t  bDescriptorSubtype;          /*!< bDescriptorSubtype: abstract control management desc */
    uint8_t  bmCapabilities;              /*!< bmCapabilities: D1 */
} usb_descriptor_acm_function_struct;

typedef struct
{
    usb_desc_header header;  /*!< descriptor header, including type and size. */
    uint8_t  bDescriptorSubtype;          /*!< bDescriptorSubtype: union func desc */
    uint8_t  bMasterInterface;            /*!< bMasterInterface: communication class interface */
    uint8_t  bSlaveInterface0;            /*!< bSlaveInterface0: data class interface */
} usb_descriptor_union_function_struct;

#pragma pack()

typedef struct
{
    usb_desc_config                config;
    usb_desc_itf                   cdc_loopback_interface;
    usb_descriptor_header_function_struct             cdc_loopback_header;
    usb_descriptor_call_managment_function_struct     cdc_loopback_call_managment;
    usb_descriptor_acm_function_struct                cdc_loopback_acm;
    usb_descriptor_union_function_struct              cdc_loopback_union;
    usb_desc_ep                    cdc_loopback_cmd_endpoint;
    usb_desc_itf                   cdc_loopback_data_interface;
    usb_desc_ep                    cdc_loopback_out_endpoint;
    usb_desc_ep                    cdc_loopback_in_endpoint;
} usb_descriptor_configuration_set_struct;

extern void* const usbd_strings[USB_STRING_COUNT];
extern const usb_desc_dev device_descriptor;
extern usb_descriptor_configuration_set_struct configuration_descriptor;

/* Function prototypes */

/*
    Initialize the CDC device
*/
void cdc_init(void);

/*
    Process CDC device communication
*/
void cdc_process(void);

/*
    Checks and retrieve new received data
    @param buffer - a pointer to an unit8_t buffer to store received data
    @param maxSize - maximum number of bytes to receive
    @return the number of received bytes stored in the buffer
*/
uint32_t cdc_getReceivedData(uint8_t * buffer, uint32_t maxSize);

/*
    Sends data through CDC connection
    @param buffer - a pointer to an unit8_t buffer with data to send
    @param size - the number of bytes to send
*/
void cdc_sendData(uint8_t * buffer, uint8_t size);

/*
    Sends a single character through CDC connection
    @param data - character to send
*/
void cdc_sendChar(char data);

/*
    Checks whether CDC device is configured or not
    @return TRUE if configured
*/
bool cdc_isConfigured(void);

/*
    Prints a text string through CDC connection
    @param string - a null terminated char string
*/
void cdc_print(char *string);


#endif  /* CDC_CORE_H */
