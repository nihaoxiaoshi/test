#include "AES.h"


/* =========================================================================== */
/*																			   */
/* -----------------ICUSer_AES_Cipher_Encryption (AES加密函数)---------------- */
/*																			   */
/* =========================================================================== */


/**
 * S盒
 */
static const uint8_t S[16][16] = { 0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

/**
 * 逆S盒
 */
static const uint8_t S2[16][16] = { 0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
	0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
	0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
	0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
	0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
	0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
	0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
	0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
	0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
	0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
	0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
	0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
	0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
	0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
	0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
	0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };

/**
 * 获取一个字节数据8位的高/左4个位
 */
static uint8_t getByteLeft4Bit(uint8_t num) 
{
	uint8_t temp = (num & 0xf0);
	return (temp >> 4);
}

/**
 * 获取一个字节数据8位的低/右4个位
 */
static uint8_t getByteRight4Bit(uint8_t num) 
{
	return (num & 0x0f);
}

/**
 * 根据索引，从S盒中获得元素(获取一个字节元素)
 */
static uint8_t getNumFromSBox(uint8_t index) 
{
	uint8_t row = getByteLeft4Bit(index);  //行
	uint8_t col = getByteRight4Bit(index); //列
	return S[row][col];
}

/**
 * 把16个元素的数组转变成4X4的数组，
 * 该矩阵中字节的排列顺序为从上到下，
 * 从左到右依次排列。
 */
static void convertTo_4x4_Array(uint8_t *source, uint8_t pArray[4][4]) 
{
	int k = 0;
	int i,j;
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++) 
		{
			pArray[j][i] = source[k];
			k++;
		}
	}
}

/**
 * 把连续的4个字节合并成一个32位比特字
 举例: uint8_t source[4] = {0x12, 0x34, 0x56, 0x78};
 	   return 0x12000000 | 0x00340000 | 0x00005600 | 0x00000078 = 0x12345678
 */
static uint32_t getU32FromU8(uint8_t *source) 
{
	return (((uint32_t)source[0] << 24) | ((uint32_t)source[1] << 16) | ((uint32_t)source[2] << 8) | (uint32_t)source[3]);
}

/**
 * 把一个4字节的数的第一、二、三、四个字节取出，
 * 放入进一个4个元素的整型数组里面。
 */
static void splitU32ToArray(uint32_t source, uint8_t *destination) 
{
	/* Copy Uint32 to Array */
	destination[0] = (uint8_t)((source >> 24u));
	destination[1] = (uint8_t)((source >> 16u) & 0xFFu);
	destination[2] = (uint8_t)((source >> 8u) & 0xFFu);
	destination[3] = (uint8_t)(source & 0xFFu);
}

/**
 * 将数组中的元素循环左移step字节
 */
static void leftLoop4Bytes(uint8_t array[4], uint8_t step) 
{
	uint8_t temp[4];
	uint8_t i;
	uint8_t index;
	for(i = 0; i < 4; i++)
	{
		temp[i] = array[i];
	}
	
	index = step % 4 == 0 ? 0 : step % 4;
	
	for(i = 0; i < 4; i++)
	{
		array[i] = temp[index];
		index++;
		index = index % 4;
	}
}

/**
 * 常量轮值表
 */
static const uint32_t Rcon[10] = { 0x01000000, 0x02000000,
								0x04000000, 0x08000000,
								0x10000000, 0x20000000,
								0x40000000, 0x80000000,
								0x1b000000, 0x36000000 };

/**
 * 密钥扩展中的T函数
 */
static uint32_t T(uint32_t num, uint8_t round) 
{
	uint8_t numArray[4];
	uint8_t i;
	uint32_t result;
	splitU32ToArray(num, numArray);
	leftLoop4Bytes(numArray, 1);//字循环左移一个字节

	//字节代换
	for(i = 0; i < 4; i++)
	{
		numArray[i] = getNumFromSBox(numArray[i]);
	}
	
	result = getU32FromU8(numArray);
	return (result ^ Rcon[round]);
}

//密钥对应的扩展数组
static uint32_t w[44];

/**
 * 扩展密钥，结果是把w[44]中的每个元素初始化
 */
static void extendKey(uint8_t *key) 
{
	int i,j;
	for(i = 0; i < 4; i++)
	{
		/* w[0] ~ w[3] */
		w[i] = getU32FromU8(key + i * 4);
	}
	
	for(i = 4, j = 0; i < 44; i++) 
	{
		if( i % 4 == 0) 
		{
			w[i] = w[i - 4] ^ T(w[i - 1], j);
			j++;//下一轮
		}
		else 
		{
			w[i] = w[i - 4] ^ w[i - 1];
		}
	}
}

/**
 * 轮密钥加
 */
static void addRoundKey(uint8_t array[4][4], uint8_t round) 
{
	uint8_t warray[4];
	uint8_t i,j;
	for(i = 0; i < 4; i++) 
	{
		splitU32ToArray(w[round * 4 + i], warray);

		for(j = 0; j < 4; j++) 
		{
			array[j][i] = array[j][i] ^ warray[j];
		}
	}
}

/**
 * 字节代换
 */
static void subBytes(uint8_t array[4][4])
{
	int i,j;
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			array[i][j] = getNumFromSBox(array[i][j]);
		}
	}
}

/**
 * 行移位
 */
static void shiftRows(uint8_t array[4][4]) 
{
	uint8_t rowTwo[4], rowThree[4], rowFour[4];
	uint8_t i;
	for(i = 0; i < 4; i++) 
	{
		rowTwo[i] = array[1][i];
		rowThree[i] = array[2][i];
		rowFour[i] = array[3][i];
	}

	leftLoop4Bytes(rowTwo, 1);
	leftLoop4Bytes(rowThree, 2);
	leftLoop4Bytes(rowFour, 3);

	for(i = 0; i < 4; i++) 
	{
		array[1][i] = rowTwo[i];
		array[2][i] = rowThree[i];
		array[3][i] = rowFour[i];
	}
}

/**
 * 列混合要用到的矩阵
 */
static const uint8_t colM[4][4] = { 
								    {2, 3, 1, 1},  // 第一行
								    {1, 2, 3, 1},  // 第二行
								    {1, 1, 2, 3},  // 第三行
								    {3, 1, 1, 2}   // 第四行
};



static uint8_t GFMul2(uint8_t s) 
{
	uint8_t a7 = s & 0x80;
	uint8_t result = s << 1;

	if(a7 != 0) 
	{
		result = result ^ 0x1b;
	}

	return result;
}

static uint8_t GFMul3(uint8_t s) 
{
	return GFMul2(s) ^ s;
}

static uint8_t GFMul4(uint8_t s) 
{
	return GFMul2(GFMul2(s));
}

static uint8_t GFMul8(uint8_t s) 
{
	return GFMul2(GFMul4(s));
}

static uint8_t GFMul9(uint8_t s) 
{
	return GFMul8(s) ^ s;
}

static uint8_t GFMul11(uint8_t s) 
{
	return GFMul9(s) ^ GFMul2(s);
}

static uint8_t GFMul12(uint8_t s) 
{
	return GFMul8(s) ^ GFMul4(s);
}

static uint8_t GFMul13(uint8_t s) 
{
	return GFMul12(s) ^ s;
}

static uint8_t GFMul14(uint8_t s) 
{
	return GFMul12(s) ^ GFMul2(s);
}

/**
 * GF上的二元运算
 */
static uint8_t GFMul(uint8_t n, uint8_t s) 
{
	uint8_t result;

	if(n == 1)
		result = s;
	else if(n == 2)
		result = GFMul2(s);
	else if(n == 3)
		result = GFMul3(s);
	else if(n == 0x9)
		result = GFMul9(s);
	else if(n == 0xb)//11
		result = GFMul11(s);
	else if(n == 0xd)//13
		result = GFMul13(s);
	else if(n == 0xe)//14
		result = GFMul14(s);

	return result;
}

/**
 * 列混合
 */
static void mixColumns(uint8_t array[4][4]) 
{
	uint8_t tempArray[4][4];
	uint8_t i,j;
	/* copy source to temp array */
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			tempArray[i][j] = array[i][j];
		}
	}
	
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			array[i][j] = GFMul(colM[i][0],tempArray[0][j]) ^ GFMul(colM[i][1],tempArray[1][j])
				^ GFMul(colM[i][2],tempArray[2][j]) ^ GFMul(colM[i][3], tempArray[3][j]);
		}
	}
}

/**
 * 检查密钥长度
 */
static uint8_t checkKeyLen(uint8_t len) 
{
	if(len == 16)
		return E_OK;
	else
		return E_NOT_OK;
}

uint8_t ICUSer_AES_Cipher_Encryption(uint8_t *plainText, uint32_t plain_length, uint8_t *cipherText, 
											uint32_t *cipher_output_size, uint8_t *key, uint8_t key_length)
{
	uint8_t pArray[4][4]; //4 x 4数组
	uint32_t k;
	uint8_t i,j;
	uint32_t outputWritten = 0;

	if(plain_length == 0 || plain_length % 16 != 0) 
	{
		printf("明文字符长度必须为16的倍数！\r\n");
		return E_NOT_OK;
	}

	if(checkKeyLen(key_length)) 
	{
		printf("密钥字符长度错误！长度必须为16。当前长度为%d\r\n",key_length);
		return E_NOT_OK;
	}

	extendKey(key);//扩展密钥
	
	for(k = 0; k < plain_length; k += 16) 
	{
		/* 把16字节原文放入4 x 4的数组中 */
		convertTo_4x4_Array(plainText + k, pArray);

		addRoundKey(pArray, 0);//一开始的轮密钥加

		/* 第1轮到第9轮的轮函数一样，包括4个操作：字节代换、行位移、列混合和轮密钥加 */
		for(i = 1; i < 10; i++)
		{

			subBytes(pArray);//字节代换

			shiftRows(pArray);//行移位

			mixColumns(pArray);//列混合

			addRoundKey(pArray, i);//轮密钥加

		}
		subBytes(pArray);//字节代换

		shiftRows(pArray);//行移位

		addRoundKey(pArray, 10);//轮密钥加


		for(i = 0; i < 4; i++)
		{
			for(j = 0; j < 4; j++)
			{
				cipherText[outputWritten] = pArray[j][i];
				outputWritten++;
			}
		}
	}

	*cipher_output_size = outputWritten;

	return E_OK;
	
}


/* =========================================================================== */
/*																			   */
/* -----------------ICUSer_AES_Cipher_Decryption (AES解密函数)---------------- */
/*																			   */
/* =========================================================================== */


/**
 * 根据索引从逆S盒中获取值
 */
static uint8_t getNumFromS2Box(uint8_t index) 
{
	uint8_t row = getByteLeft4Bit(index);  //行
	uint8_t col = getByteRight4Bit(index); //列
	return S2[row][col];
}

/**
 * 逆字节变换
 */
static void deSubBytes(uint8_t array[4][4]) 
{
	uint8_t i,j;
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			array[i][j] = getNumFromS2Box(array[i][j]);
		}
	}
}

/**
 * 把4个元素的数组循环右移step个字节
 */
static void rightLoop4Bytes(uint8_t array[4], uint8_t step) 
{
	uint8_t temp[4];
	int8_t i;
	int8_t index;
	for(i = 0; i < 4; i++)
	{
		temp[i] = array[i];
	}
	
	index = step % 4 == 0 ? 0 : step % 4;
	index = 3 - index;
	
	for(i = 3; i >= 0; i--) 
	{
		array[i] = temp[index];
		index--;
		index = index == -1 ? 3 : index;
	}
}

/**
 * 逆行移位
 */
static void deShiftRows(uint8_t array[4][4]) 
{
	uint8_t rowTwo[4], rowThree[4], rowFour[4];//行
	uint8_t i;
	for(i = 0; i < 4; i++) 
	{
		rowTwo[i] = array[1][i];
		rowThree[i] = array[2][i];
		rowFour[i] = array[3][i];
	}

	rightLoop4Bytes(rowTwo, 1);
	rightLoop4Bytes(rowThree, 2);
	rightLoop4Bytes(rowFour, 3);

	for(i = 0; i < 4; i++) 
	{
		array[1][i] = rowTwo[i];
		array[2][i] = rowThree[i];
		array[3][i] = rowFour[i];
	}
}

/**
 * 逆列混合用到的矩阵
 */
static const uint8_t deColM[4][4] = { 
                                      {0xe, 0xb, 0xd, 0x9},
									  {0x9, 0xe, 0xb, 0xd},
									  {0xd, 0x9, 0xe, 0xb},
									  {0xb, 0xd, 0x9, 0xe} 
};

/**
 * 逆列混合
 */
static void deMixColumns(uint8_t array[4][4]) 
{
	uint8_t tempArray[4][4];
	uint8_t i,j;
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			tempArray[i][j] = array[i][j];
		}
	}

	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			array[i][j] = GFMul(deColM[i][0],tempArray[0][j]) ^ GFMul(deColM[i][1],tempArray[1][j])
				^ GFMul(deColM[i][2],tempArray[2][j]) ^ GFMul(deColM[i][3], tempArray[3][j]);
		}
	}
}

/**
 * 把两个4X4数组进行异或，异或后的值存放在aArray
 */
static void addRoundTowArray(uint8_t aArray[4][4],uint8_t bArray[4][4]) 
{
	uint8_t i,j;
	for(i = 0; i < 4; i++) 
	{
		for(j = 0; j < 4; j++) 
		{
			aArray[j][i] = aArray[j][i] ^ bArray[j][i];
		}
	}
}

/**
 * 从4个32位的密钥字中获得4X4数组，
 * 用于进行逆列混合
 */
static void getArrayFrom4W(uint8_t i, uint8_t array[4][4]) 
{
	uint8_t index,j;
	uint8_t colOne[4], colTwo[4], colThree[4], colFour[4];//列
	index = i * 4;
	
	splitU32ToArray(w[index], colOne);
	splitU32ToArray(w[index + 1], colTwo);
	splitU32ToArray(w[index + 2], colThree);
	splitU32ToArray(w[index + 3], colFour);

	for(j = 0; j < 4; j++) 
	{
		array[j][0] = colOne[j];
		array[j][1] = colTwo[j];
		array[j][2] = colThree[j];
		array[j][3] = colFour[j];
	}

}

#if 0
/* 第一种解密流程 */
uint8_t ICUSer_AES_Cipher_Decryption(uint8_t *cipherText, uint32_t cipher_length, uint8_t *plainText, 
											uint32_t *source_output_size, uint8_t *key, uint8_t key_length)
{
	
	uint8_t pArray[4][4]; //4 x 4数组
	uint8_t wArray[4][4]; //密钥4 x 4数组
	uint32_t k;
	uint8_t i,j;
	uint32_t outputWritten = 0;

	if(cipher_length == 0 || cipher_length % 16 != 0) 
	{
		printf("密文字符长度必须为16的倍数！\r\n");
		return E_NOT_OK;
	}

	if(checkKeyLen(key_length)) 
	{
		printf("密钥字符长度错误！长度必须为16。当前长度为%d\r\n",key_length);
		return E_NOT_OK;
	}

	extendKey(key);//扩展密钥

	for(k = 0; k < cipher_length; k += 16) 
	{
		/* 把16字节密文放入4 x 4的数组中 */
		convertTo_4x4_Array(cipherText + k, pArray);	

		addRoundKey(pArray, 10);//轮密钥加

		for(i = 9; i >= 1; i--) 
		{
			deSubBytes(pArray);//逆字节变换

			deShiftRows(pArray);//逆行移位

			deMixColumns(pArray);//逆列混合
			
			getArrayFrom4W(i, wArray);//获取第i轮的密钥
			
			deMixColumns(wArray);//对密钥逆列混合

			addRoundTowArray(pArray, wArray);//轮密钥加
		}

		deSubBytes(pArray);//逆字节变换

		deShiftRows(pArray);//逆行移位

		addRoundKey(pArray, 0);//轮密钥加

		for(i = 0; i < 4; i++)
		{
			for(j = 0; j < 4; j++)
			{
				plainText[outputWritten] = pArray[j][i];
				outputWritten++;
			}
		}

	}

	*source_output_size = outputWritten;

	return E_OK;

}
#else
/* 第二种解密流程 */
uint8_t ICUSer_AES_Cipher_Decryption(uint8_t *cipherText, uint32_t cipher_length, uint8_t *plainText, 
											uint32_t *source_output_size, uint8_t *key, uint8_t key_length)
{
	
	uint8_t pArray[4][4]; //4 x 4数组
	uint8_t wArray[4][4]; //密钥4 x 4数组
	uint32_t k;
	uint8_t i,j;
	uint32_t outputWritten = 0;

	if(cipher_length == 0 || cipher_length % 16 != 0) 
	{
		printf("密文字符长度必须为16的倍数！\r\n");
		return E_NOT_OK;
	}

	if(checkKeyLen(key_length)) 
	{
		printf("密钥字符长度错误！长度必须为16。当前长度为%d\r\n",key_length);
		return E_NOT_OK;
	}

	extendKey(key);//扩展密钥

	for(k = 0; k < cipher_length; k += 16) 
	{
		/* 把16字节密文放入4 x 4的数组中 */
		convertTo_4x4_Array(cipherText + k, pArray);	

		addRoundKey(pArray, 10);//轮密钥加

		for(i = 9; i >= 1; i--) 
		{
			deShiftRows(pArray);//逆行移位
			
			deSubBytes(pArray);//逆字节变换
			
			addRoundKey(pArray, i);//轮密钥加
			
			deMixColumns(pArray);//逆列混合
		}

		deShiftRows(pArray);//逆行移位
					
		deSubBytes(pArray);//逆字节变换
		
		addRoundKey(pArray, 0);//轮密钥加

		for(i = 0; i < 4; i++)
		{
			for(j = 0; j < 4; j++)
			{
				plainText[outputWritten] = pArray[j][i];
				outputWritten++;
			}
		}

	}

	*source_output_size = outputWritten;

	return E_OK;

}
#endif

