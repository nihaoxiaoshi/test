#include "MyFlash.h"
#include "stm32f10x_flash.h"

uint32_t MyFlash_ReadWord(uint32_t Address)
{
	return (*(__IO uint32_t*)Address);
}

uint16_t MyFlash_ReadHalfWord(uint32_t Address)
{
	return (*(__IO uint16_t*)Address);
}

uint8_t MyFlash_ReadByte(uint32_t Address)
{
	return (*(__IO uint8_t*)Address);
}

void MyFlash_EraseAllPages(void)
{
	FLASH_Unlock();
	FLASH_EraseAllPages();
	FLASH_Lock();
}

void MyFlash_ErasePage(uint32_t Address)
{
	FLASH_Unlock();
	FLASH_ErasePage(Address);
	FLASH_Lock();
}

void MyFlash_ProgramWord(uint32_t Address, uint32_t Data)
{
	FLASH_Unlock();
	FLASH_ProgramWord(Address,Data);
	FLASH_Lock();
}

void MyFlash_ProgramHalfWord(uint32_t Address, uint16_t Data)
{
	FLASH_Unlock();
	FLASH_ProgramHalfWord(Address,Data);
	FLASH_Lock();
}
