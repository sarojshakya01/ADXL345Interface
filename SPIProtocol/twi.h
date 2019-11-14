/**
 ******************************************************************************
 * @file	twi.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-02-13
 * @brief	Contains function prototypes, constants to manage the TWI-peripheral
 *			on ATmega328x
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef TWI_H_
#define TWI_H_

/* Includes ------------------------------------------------------------------*/
/* Defines -------------------------------------------------------------------*/
/* Typedefs ------------------------------------------------------------------*/
/**
 * @brief  TWI prescaler
 */
typedef enum
{
    TWI_PRESCALER_1 =   1,
    TWI_PRESCALER_4 =   4,
    TWI_PRESCALER_16 =  16,
    TWI_PRESCALER_64 =  64
} TWI_Prescaler_TypeDef;
#define IS_TWI_PRESCALER(PRESCALER) (((PRESCALER) == TWI_PRESCALER_1) || ((PRESCALER) == TWI_PRESCALER_4) || \
                                    ((PRESCALER) == TWI_PRESCALER_16) || ((PRESCALER) == TWI_PRESCALER_64))

/**
 * @brief  TWI frequency
 */
typedef uint32_t TWI_Frequency_TypeDef;
#define IS_TWI_FREQUENCY(FREQUENCY) ((FREQUENCY <= 400000) && (FREQUENCY >= 50000))

/**
 * @brief  TWI mode
 */
typedef enum
{
    TWI_MODE_MASTER =   0x00,
    TWI_MODE_SLAVE =    0x01,
} TWI_Mode_TypeDef;
#define IS_TWI_MODE(MODE) (((MODE) == TWI_MODE_MASTER) || ((MODE) == TWI_MODE_SLAVE))

/**
 * @brief  TWI Init structure definition
 */
typedef struct
{
    TWI_Prescaler_TypeDef TWI_Prescaler;		/** Specifies the prescaler for the TWI peripheral.
                                                    This parameter can be any value of TWI_Prescaler_TypeDef */
    TWI_Frequency_TypeDef TWI_Frequency;        /** Specifies the SCL frequency for the TWI peripheral.
                                                    This parameter can be any value of TWI_Frequency_TypeDef */
    TWI_Mode_TypeDef TWI_Mode;                  /** Specifies which mode the TWI peripheral should be used in
                                                    This parameter can be any value of TWI_Frequency_TypeDef */
    
} TWI_Init_TypeDef;

/* Function prototypes -------------------------------------------------------*/
void TWI_Init(TWI_Init_TypeDef *TWI_InitStruct);
void TWI_InitStandard();
void TWI_Start();
void TWI_Stop();
uint8_t TWI_ReadAck();
uint8_t TWI_ReadNack();
uint8_t TWI_GetStatus();
void TWI_WriteRaw(const uint8_t data);

uint8_t TWI_BeginTransmission(const uint8_t address);
void TWI_EndTransmission();
uint8_t TWI_Write(const uint8_t data);
uint8_t TWI_RequestFrom(const uint8_t address, uint8_t* storage, const uint8_t NumByteToRead);

uint8_t TWI_SlaveAtAddress(const uint8_t Address);
uint8_t TWI_Initialized();

#endif /* TWI_H_ */