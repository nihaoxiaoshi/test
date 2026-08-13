#include "flash.h"
#include "stm32f10x_flash.h"
#include <string.h>

FlashDrv_Status FlashDrv_Unlock(void)
{
    FLASH_Unlock();
    return FLASH_DRV_OK;
}

FlashDrv_Status FlashDrv_Lock(void)
{
    FLASH_Lock();
    return FLASH_DRV_OK;
}

FlashDrv_Status FlashDrv_ErasePage(uint32_t page_addr)
{
    if (page_addr < 0x08000000 || page_addr >= 0x08010000)
        return FLASH_DRV_ERROR_PARAM;

    FLASH_Status status = FLASH_ErasePage(page_addr);
    if (status == FLASH_COMPLETE)
        return FLASH_DRV_OK;

    return FLASH_DRV_ERROR_ERASE;
}

FlashDrv_Status FlashDrv_WriteDID(uint16_t did, const uint8_t *data, uint16_t len)
{
    uint32_t addr = DID_FLASH_BASE_ADDR;
    uint32_t i;
    uint16_t halfword;

    if (data == NULL || len == 0)
        return FLASH_DRV_ERROR_PARAM;

    if (len > DID_FLASH_PAGE_SIZE - 8)
        return FLASH_DRV_ERROR_PARAM;

    if (FlashDrv_Unlock() != FLASH_DRV_OK)
        return FLASH_DRV_ERROR_WRITE;

    if (FlashDrv_ErasePage(addr) != FLASH_DRV_OK)
    {
        FlashDrv_Lock();
        return FLASH_DRV_ERROR_ERASE;
    }

    if (FLASH_ProgramWord(addr, DID_FLASH_MAGIC) != FLASH_COMPLETE)
    {
        FlashDrv_Lock();
        return FLASH_DRV_ERROR_WRITE;
    }
    addr += 4;

    if (FLASH_ProgramHalfWord(addr, did) != FLASH_COMPLETE)
    {
        FlashDrv_Lock();
        return FLASH_DRV_ERROR_WRITE;
    }
    addr += 2;

    if (FLASH_ProgramHalfWord(addr, len) != FLASH_COMPLETE)
    {
        FlashDrv_Lock();
        return FLASH_DRV_ERROR_WRITE;
    }
    addr += 2;

    for (i = 0; i < len; i += 2)
    {
        halfword = data[i];
        if (i + 1 < len)
            halfword |= (uint16_t)data[i + 1] << 8;
        else
            halfword |= 0xFF00;

        if (FLASH_ProgramHalfWord(addr + i, halfword) != FLASH_COMPLETE)
        {
            FlashDrv_Lock();
            return FLASH_DRV_ERROR_WRITE;
        }
    }

    FlashDrv_Lock();
    return FLASH_DRV_OK;
}

FlashDrv_Status FlashDrv_ReadDID(uint16_t did, uint8_t *data, uint16_t *len)
{
    uint32_t addr = DID_FLASH_BASE_ADDR;
    uint32_t magic;
    uint16_t stored_did;
    uint16_t stored_len;
    uint32_t i;

    if (data == NULL || len == NULL)
        return FLASH_DRV_ERROR_PARAM;

    magic = *(volatile uint32_t *)addr;
    if (magic != DID_FLASH_MAGIC)
        return FLASH_DRV_ERROR_NO_DATA;

    stored_did = *(volatile uint16_t *)(addr + 4);
    if (stored_did != did)
        return FLASH_DRV_ERROR_NO_DATA;

    stored_len = *(volatile uint16_t *)(addr + 6);
    if (stored_len > DID_FLASH_PAGE_SIZE - 8)
        return FLASH_DRV_ERROR_NO_DATA;

    for (i = 0; i < stored_len; i++)
        data[i] = *(volatile uint8_t *)(addr + 8 + i);

    *len = stored_len;
    return FLASH_DRV_OK;
}
