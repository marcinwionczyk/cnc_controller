/**
  ******************************************************************************
  * File Name          : I2C.h
  * Description        : This file provides code for the configuration
  *                      of the I2C instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __i2c_H
#define __i2c_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN Private defines */
#define EE_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define EE_LONG_TIMEOUT         ((uint32_t)(30 * EE_FLAG_TIMEOUT))
#define I2C_REQUEST_READ                        0x01
#define I2C_SLAVE_ADDR                          0xA0
/* USER CODE END Private defines */

void MX_I2C1_Init(void);

/* USER CODE BEGIN Prototypes */
//typedef struct {
//    uint8_t SB:1;
//    uint8_t ADDR:1;
//    uint8_t STOP:1;
//    uint8_t BTF:1;
//    uint8_t RXNE:1;
//    uint8_t TXE:1;
//    uint8_t BUSY:1;
//    uint8_t MSL:1;
//} SEventFlags;
//
//typedef struct {
//    uint8_t BEER:1; //Bus Error detection
//    uint8_t ARLO:1; //Arbitration Loss
//    uint8_t AF:1;
//    uint8_t OVR:1;
//    uint8_t PEC_ERR:1;
//} SErrorFlags;
//
//void I2C_EVT_callback(void);
//void I2C_ERR_callback(void);
//uint8_t I2C_ReadByte(uint16_t register_addr);
//uint8_t I2C_WriteByte(uint16_t register_addr, uint8_t data);
//
//uint8_t I2C_ReadByteArray(uint16_t register_addr, uint8_t *pData, uint16_t Len);
//uint8_t I2C_WriteByteArray(uint16_t register_addr, uint8_t *pData, uint16_t Len);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ i2c_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
