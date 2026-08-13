#include "Store.h"

uint16_t Store_Data[STORE_NUM];


void Store_Init(void)
{
	if(MyFlash_ReadHalfWord(STORE_START_ADDRESS) != 0xA5A5)
	{
		MyFlash_ErasePage(STORE_START_ADDRESS);
		MyFlash_ProgramHalfWord(STORE_START_ADDRESS, 0xA5A5);
	}
	for(uint16_t Index = 0; Index < STORE_NUM; Index++)
	{
		Store_Data[Index] = MyFlash_ReadHalfWord(STORE_START_ADDRESS + (Index * 2));
	}
}
void Store_Save(void)
{
	MyFlash_ErasePage(STORE_START_ADDRESS);
	for(uint16_t Index = 0; Index < STORE_NUM; Index++)
	{
		MyFlash_ProgramHalfWord(STORE_START_ADDRESS + (Index * 2), Store_Data[Index]);
	}
}

void Store_Clear(void)
{
	for(uint16_t Index = 1; Index < STORE_NUM; Index++)
	{
		Store_Data[Index] = 0x0000;
	}
	Store_Save();
}
