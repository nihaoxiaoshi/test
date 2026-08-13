#ifndef __FLASH_DRV_H
#define __FLASH_DRV_H

#include <stdint.h>

#define DID_FLASH_BASE_ADDR   0x0800FC00
#define DID_FLASH_PAGE_SIZE   1024
#define DID_FLASH_MAGIC       0xA55AA55A

typedef enum {
    FLASH_DRV_OK = 0,
    FLASH_DRV_ERROR_PARAM,
    FLASH_DRV_ERROR_ERASE,
    FLASH_DRV_ERROR_WRITE,
    FLASH_DRV_ERROR_NO_DATA,
    FLASH_DRV_ERROR_TIMEOUT
} FlashDrv_Status;

FlashDrv_Status FlashDrv_Unlock(void);
FlashDrv_Status FlashDrv_Lock(void);
FlashDrv_Status FlashDrv_ErasePage(uint32_t page_addr);
FlashDrv_Status FlashDrv_WriteDID(uint16_t did, const uint8_t *data, uint16_t len);
FlashDrv_Status FlashDrv_ReadDID(uint16_t did, uint8_t *data, uint16_t *len);

#endif
