#ifndef __MY_AES_H__
#define __MY_AES_H__

#include "usart.h"

#define E_OK      0u
#define E_NOT_OK  1u


/**
 * 参数 plain: 明文数组。
 * 参数 plain_length: 明文的长度,长度必须为16的倍数。
 * 参数 cipher: 密文数组。
 * 参数 cipher_output_size: 加密后的密文长度。
 * 参数 key: 密钥数组(AES-128/16bytes)。
 * 参数 key_length: 密钥长度。
 */
 
 /* AES加密模式：AES-128(密钥长度16字节) ECB，填充方式：NoPadding，测试稳定 */
uint8_t ICUSer_AES_Cipher_Encryption(uint8_t *plainText, uint32_t plain_length, uint8_t *cipherText, 
											uint32_t *cipher_output_size, uint8_t *key, uint8_t key_length);


/**
 * 参数 cipher: 密文数组。
 * 参数 cipher_length: 密文的长度,长度必须为16的倍数。
 * 参数 plain: 明文数组。
 * 参数 source_output_size: 解密后的明文长度。
 * 参数 key: 密钥数组(AES-128/16bytes)。
 * 参数 key_length: 密钥长度。
 */
 
 /* AES解密模式：AES-128(密钥长度16字节) ECB，填充方式：NoPadding，测试稳定 */
uint8_t ICUSer_AES_Cipher_Decryption(uint8_t *cipherText, uint32_t cipher_length, uint8_t *plainText, 
											uint32_t *source_output_size, uint8_t *key, uint8_t key_length);


#endif

