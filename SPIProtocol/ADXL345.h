/**
 ******************************************************************************
 * @file	ADXL345.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-03-05
 * @brief	Contains function prototypes for the ADXL345 accelerometer
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ADXL345_H_
#define ADXL345_H_

/* Includes ------------------------------------------------------------------*/
/* Defines -------------------------------------------------------------------*/
#define ADXL345_ADDRESS_ALT_HIGH	(0x1D)
#define ADXL345_ADDRESS_ALT_LOW		(0x53)

//#define ADXL345_DEBUG

/* Typedefs ------------------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/
uint8_t ADXL345_Init(const uint8_t Address);
void ADXL345_Test();

#ifdef ADXL345_DEBUG
void ADXL345_DebugAllRegisters();
#endif

#endif /* ADXL345_H_ */