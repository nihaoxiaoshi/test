#ifndef __BL_CONFIG_H
#define __BL_CONFIG_H

/* Flash分区地址 */
#define APP_ADDR        0x08004000  /* APP起始地址 */
#define APP_SIZE        0x00005000  /* APP大小 47KB */
#define BL_ADDR         0x08000000  /* Bootloader起始地址 */
#define BL_SIZE         0x00004000  /* Bootloader大小 16KB */
#define STORE_ADDR      0x0800FC00  /* NVM存储地址 */
#define STORE_SIZE      0x00000400  /* NVM大小 1KB */

/* Bootloader模式选择标志 - 存放在RAM末尾 */
#define BL_FLAG_ADDR    0x0800F400  /* RAM最后一个字 */
#define BL_FLAG_VALUE   0x5A5A5A5A  /* 写入此值则下次启动进入Bootloader */

/* APP有效性校验标志 - 存放在APP末尾 */
#define APP_VALID_FLAG_ADDR  0x0800F800  /* APP区域最后4字节 */
#define APP_VALID_MAGIC      0xA5A5A5A5  /* APP有效标记 */

#endif
