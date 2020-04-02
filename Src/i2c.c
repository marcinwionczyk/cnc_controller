/**
  ******************************************************************************
  * File Name          : I2C.c
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

/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* USER CODE BEGIN 0 */
//static volatile uint32_t EE_Timeout = EE_LONG_TIMEOUT;
//static volatile SEventFlags i2c_event_flags = {0};
//static volatile SErrorFlags i2c_error_flags = {0};
//static volatile uint8_t i2c_event = 0;
/* USER CODE END 0 */

I2C_HandleTypeDef hi2c1;

/* I2C1 init function */
void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */
  
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C1 GPIO Configuration    
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C1 clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
  /* USER CODE BEGIN I2C1_MspInit 1 */

  /* USER CODE END I2C1_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();
  
    /**I2C1 GPIO Configuration    
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);

  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */

//static inline void event_master_mode_select(){
//    i2c_event = i2c_event_flags.BUSY && i2c_event_flags.MSL && i2c_event_flags.SB;
//}
//
//static inline void event_master_transmitter_mode_selected(){
//    i2c_event = i2c_event_flags.BUSY && i2c_event_flags.MSL && i2c_event_flags.ADDR && i2c_event_flags.TXE;
//}
//
//static inline void event_master_byte_transmitting(){
//    i2c_event = i2c_event_flags.BUSY && i2c_event_flags.MSL && i2c_event_flags.TXE;
//}
//
//static inline void event_master_byte_transmitted(){
//    i2c_event = i2c_event_flags.BUSY && i2c_event_flags.MSL && i2c_event_flags.TXE && i2c_event_flags.BTF;
//}
//
//
//void I2C_EVT_callback(){
//    i2c_event_flags.SB = LL_I2C_IsActiveFlag_SB(I2C1);
//    i2c_event_flags.ADDR = LL_I2C_IsActiveFlag_ADDR(I2C1);
//    i2c_event_flags.STOP = LL_I2C_IsActiveFlag_STOP(I2C1);
//    i2c_event_flags.BTF = LL_I2C_IsActiveFlag_BTF(I2C1);
//    i2c_event_flags.RXNE = LL_I2C_IsActiveFlag_RXNE(I2C1);
//    i2c_event_flags.TXE = LL_I2C_IsActiveFlag_TXE(I2C1);
//    i2c_event_flags.BUSY = LL_I2C_IsActiveFlag_BUSY(I2C1);
//    i2c_event_flags.MSL = LL_I2C_IsActiveFlag_MSL(I2C1);
//}
//
//void I2C_ERR_callback(){
//    i2c_error_flags.AF = LL_I2C_IsActiveFlag_AF(I2C1);
//    i2c_error_flags.ARLO = LL_I2C_IsActiveFlag_ARLO(I2C1);
//    i2c_error_flags.BEER = LL_I2C_IsActiveFlag_BERR(I2C1);
//    i2c_error_flags.OVR = LL_I2C_IsActiveFlag_OVR(I2C1);
//    i2c_error_flags.PEC_ERR = LL_I2C_IsActiveSMBusFlag_PECERR(I2C1);
//    Error_Handler();
//}
//
//uint8_t I2C_ReadByte(uint16_t register_addr){
//    EE_Timeout = EE_FLAG_TIMEOUT;
//    i2c_event = 0;
//    uint8_t retVal;
//    uint8_t slave_addr = I2C_SLAVE_ADDR | ((register_addr & 0x700) >> 7);
//    /*!< While the bus is busy */
//    while(i2c_event_flags.BUSY){
//        if((EE_Timeout--) == 0) return 0xFF;
//    }
//    /*!< Send START condition */
//    LL_I2C_GenerateStartCondition(I2C1);
//    EE_Timeout = EE_FLAG_TIMEOUT;
//    while(!i2c_event){
//        if((EE_Timeout--) == 0){
//            LL_I2C_GenerateStopCondition(I2C1);
//            return 0xFF;
//        }
//        event_master_mode_select();
//    }
//    LL_I2C_TransmitData8(I2C1, slave_addr);
//    EE_Timeout = EE_FLAG_TIMEOUT;
//    i2c_event = 0;
//    while(!i2c_event){
//        if((EE_Timeout--) == 0){
//            /*!< Send STOP Condition */
//            LL_I2C_GenerateStopCondition(I2C1);
//            return 0xFF;
//        }
//        event_master_transmitter_mode_selected();
//    }
//    LL_I2C_TransmitData8(I2C1, (uint8_t) register_addr);
//    EE_Timeout = EE_FLAG_TIMEOUT;
//    while(!i2c_event_flags.BTF){
//        if((EE_Timeout--) == 0){
//            /*!< Send STOP Condition */
//            LL_I2C_GenerateStopCondition(I2C1);
//            return 0xFF;
//        }
//    }
//    /* Send start condition second time */
//    LL_I2C_GenerateStartCondition(I2C1);
//    i2c_event = 0;
//    while(!i2c_event){
//        if((EE_Timeout--) == 0){
//            LL_I2C_GenerateStopCondition(I2C1);
//            return 0xFF;
//        }
//        event_master_mode_select();
//    }
//    LL_I2C_TransmitData8(I2C1, slave_addr | I2C_REQUEST_READ);
//    EE_Timeout = EE_FLAG_TIMEOUT;
//    while(!i2c_event_flags.ADDR){
//        if((EE_Timeout--) == 0){
//            /*!< Send STOP Condition */
//            LL_I2C_GenerateStopCondition(I2C1);
//            return 0xFF;
//        }
//    }
//    LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);
//    LL_I2C_ClearFlag_ADDR(I2C1);
//    i2c_event_flags.ADDR = 0;
//    EE_Timeout = EE_FLAG_TIMEOUT;
//    while(!i2c_event_flags.RXNE){
//        if((EE_Timeout--) == 0) return 0xFF;
//    }
//    retVal = LL_I2C_ReceiveData8(I2C1);
//    LL_I2C_GenerateStopCondition(I2C1);
//    EE_Timeout = EE_FLAG_TIMEOUT;
//    while(i2c_event_flags.STOP){
//        if((EE_Timeout--) == 0) return 0xFF;
//    }
//    LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
//    return retVal;
//}
//
//uint8_t I2C_WriteByte(uint16_t register_addr, uint8_t data){
//    EE_Timeout = EE_LONG_TIMEOUT;
//    i2c_event = 0;
//    uint8_t slave_addr = I2C_SLAVE_ADDR | ((register_addr & 0x700) >> 7);
//    /*!< While the bus is busy */
//    while(i2c_event_flags.BUSY){
//        if((EE_Timeout--) == 0) return 1;
//    }
//    LL_I2C_GenerateStartCondition(I2C1);
//    EE_Timeout = EE_FLAG_TIMEOUT;
//
//    while(!i2c_event){
//        if((EE_Timeout--) == 0){
//            LL_I2C_GenerateStopCondition(I2C1);
//            return 2;
//        }
//        event_master_mode_select();
//    }
//    EE_Timeout = EE_FLAG_TIMEOUT;
//    LL_I2C_TransmitData8(I2C1, slave_addr);
//    i2c_event = 0;
//    while(!i2c_event){
//        if((EE_Timeout--) == 0) {
//            LL_I2C_GenerateStopCondition(I2C1);
//            return 3;
//        }
//        event_master_transmitter_mode_selected();
//    }
//    LL_I2C_TransmitData8(I2C1, (uint8_t) register_addr);
//    EE_Timeout = EE_FLAG_TIMEOUT;
//    i2c_event = 0;
//    while(!i2c_event){
//        if((EE_Timeout--) == 0){
//            LL_I2C_GenerateStopCondition(I2C1);
//            return 4;
//        }
//        event_master_byte_transmitting();
//    }
//    LL_I2C_TransmitData8(I2C1, data);
//    EE_Timeout = EE_FLAG_TIMEOUT;
//    i2c_event = 0;
//    while(!i2c_event){
//        if((EE_Timeout--) == 0)
//        {
//            /*!< Send STOP Condition */
//            LL_I2C_GenerateStopCondition(I2C1);
//            return 5;
//        }
//        event_master_byte_transmitted();
//    }
//    LL_I2C_GenerateStopCondition(I2C1);
//    return 0;
//}
//
//uint8_t I2C_ReadByteArray(uint16_t register_addr, uint8_t *pData, uint16_t Len){
//    uint8_t retVal = 0;
//    i2c_event = 0;
//    uint8_t slave_addr = I2C_SLAVE_ADDR | ((register_addr & 0x700) >> 7);
//    EE_Timeout = EE_LONG_TIMEOUT;
//    while(i2c_event_flags.BUSY){
//        if((EE_Timeout--) == 0) return 0xFF;
//    }
//    LL_I2C_GenerateStartCondition(I2C1);
//    EE_Timeout = EE_FLAG_TIMEOUT;
//
//    while(!i2c_event){
//        if((EE_Timeout--) == 0){
//            LL_I2C_GenerateStopCondition(I2C1);
//            return 0xFF;
//        }
//        event_master_mode_select();
//    }
//    LL_I2C_TransmitData8(I2C1, slave_addr);
//    EE_Timeout = EE_FLAG_TIMEOUT;
//    i2c_event = 0;
//    while(!i2c_event){
//        if((EE_Timeout--) == 0){
//            LL_I2C_GenerateStopCondition(I2C1);
//            return 0xFF;
//        }
//        event_master_transmitter_mode_selected();
//    }
//    LL_I2C_TransmitData8(I2C1, (uint8_t) register_addr);
//    EE_Timeout = EE_FLAG_TIMEOUT;
//    while(!i2c_event_flags.BTF){
//        if((EE_Timeout--) == 0){
//            LL_I2C_GenerateStopCondition(I2C1);
//            return 0xFF;
//        }
//    }
//    LL_I2C_GenerateStartCondition(I2C1);
//    EE_Timeout = EE_FLAG_TIMEOUT;
//    i2c_event = 0;
//    while(!i2c_event){
//        if((EE_Timeout--) == 0){
//            LL_I2C_GenerateStopCondition(I2C1);
//            return 0xFF;
//        }
//        event_master_mode_select();
//    }
//    LL_I2C_TransmitData8(I2C1, slave_addr | I2C_REQUEST_READ);
//    EE_Timeout = EE_FLAG_TIMEOUT;
//    while(!i2c_event_flags.ADDR){
//        if((EE_Timeout--) == 0){
//            /*!< Send STOP Condition */
//            LL_I2C_GenerateStopCondition(I2C1);
//            return 0xFF;
//        }
//    }
//    for(int i=0; i < Len; i++){
//        if(i == Len - 1){
//            LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);
//        }
//        LL_I2C_ClearFlag_ADDR(I2C1);
//        i2c_event_flags.ADDR = 0;
//        EE_Timeout = EE_FLAG_TIMEOUT;
//        while(!i2c_event_flags.RXNE){
//            if((EE_Timeout--) == 0) return 0xFF;
//        }
//        pData[i] = LL_I2C_ReceiveData8(I2C1);
//    }
//    LL_I2C_GenerateStopCondition(I2C1);
//    EE_Timeout = EE_FLAG_TIMEOUT;
//    while(!i2c_event_flags.STOP){
//        if((EE_Timeout--) == 0) return 0xFF;
//    }
//    LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
//    return retVal;
//}
//
//uint8_t I2C_WriteByteArray(uint16_t register_addr, uint8_t *pData, uint16_t Len){
//    uint8_t slave_addr = I2C_SLAVE_ADDR | ((register_addr & 0x700) >> 7);
//    EE_Timeout = EE_LONG_TIMEOUT;
//    i2c_event = 0;
//    while(i2c_event_flags.BUSY){
//        if((EE_Timeout--) == 0) return 1;
//    }
//    LL_I2C_GenerateStartCondition(I2C1);
//    EE_Timeout = EE_FLAG_TIMEOUT;
//    while(!i2c_event){
//        if((EE_Timeout--) == 0)
//        {
//            /*!< Send STOP Condition */
//            LL_I2C_GenerateStopCondition(I2C1);
//            return 2;
//        }
//        event_master_mode_select();
//    }
//    /*!< Send EEPROM address for write */
//    EE_Timeout = EE_FLAG_TIMEOUT;
//    LL_I2C_TransmitData8(I2C1, slave_addr);
//    i2c_event = 0;
//    while(!i2c_event){
//        if((EE_Timeout--) == 0)
//        {
//            /*!< Send STOP Condition */
//            LL_I2C_GenerateStopCondition(I2C1);
//            return 3;
//        }
//        event_master_transmitter_mode_selected();
//    }
//    EE_Timeout = EE_FLAG_TIMEOUT;
//    LL_I2C_TransmitData8(I2C1, (uint8_t) register_addr);
//    i2c_event = 0;
//    while(!i2c_event){
//        if((EE_Timeout--) == 0){
//            LL_I2C_GenerateStopCondition(I2C1);
//            return 4;
//        }
//        event_master_byte_transmitting();
//    }
//    for(int i = 0; i < Len; i++){
//        LL_I2C_TransmitData8(I2C1, pData[i]);
//        EE_Timeout = EE_FLAG_TIMEOUT;
//        i2c_event = 0;
//        while(!i2c_event){
//            if((EE_Timeout--) == 0){
//                LL_I2C_GenerateStopCondition(I2C1);
//                return 3;
//            }
//            event_master_byte_transmitted();
//        }
//    }
//    LL_I2C_GenerateStopCondition(I2C1);
//    return 0;
//}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
