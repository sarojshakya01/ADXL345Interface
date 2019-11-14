/* Includes ------------------------------------------------------------------*/
#include <avr/io.h>
#include <util/twi.h>
#include <assert.h>
#include "twi.h"

/* Private defines -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t _twiInitStatus;
/* Private functions ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/**
 * @brief	Initializes the TWI peripheral according to the specified parameters in the TWI_InitStruct.
 * @param	TWI_InitStruct: pointer to a TWI_Init_TypeDef structure that contains
 *			the configuration information for the TWI peripheral.
 * @retval	None
 */
void TWI_Init(TWI_Init_TypeDef *TWI_InitStruct)
{
    // Check parameters
   // assert_param(IS_TWI_PRESCALER(TWI_InitStruct->TWI_Prescaler));
    //assert_param(IS_TWI_FREQUENCY(TWI_InitStruct->TWI_Frequency));
    
    // Calculate bit rate
    int16_t bitRateTest = (F_CPU - 16 * TWI_InitStruct->TWI_Frequency) /
							(2 * TWI_InitStruct->TWI_Prescaler * TWI_InitStruct->TWI_Frequency);
    // Check to make sure the bitrate is valid
    if (bitRateTest > 0xFF || bitRateTest <= 0)
	{
		while (1);
	}
    uint8_t bitRate = (uint8_t)bitRateTest;

    // Set SCL-period
	switch (TWI_InitStruct->TWI_Prescaler)
	{
	case TWI_PRESCALER_1:
		TWSR = 0x00;
		break;
	case TWI_PRESCALER_4:
		TWSR = 0x01;
		break;
	case TWI_PRESCALER_16:
		TWSR = 0x02;
		break;
	case TWI_PRESCALER_64:
		TWSR = 0x03;
		break;
	}
	
	TWBR = bitRate;
	
	// Enable TWI
	TWCR = _BV(TWEN);
	_twiInitStatus = 1;
}

/**
 * @brief	Initializes the TWI peripheral as Master, 400 kHz
 * @param	None
 * @retval	None
 */
void TWI_InitStandard()
{
	if (!TWI_Initialized())
	{
		TWI_Init_TypeDef twiInit;
		twiInit.TWI_Frequency = 400000;
		twiInit.TWI_Mode = TWI_MODE_MASTER;
		twiInit.TWI_Prescaler = TWI_PRESCALER_1;
		TWI_Init(&twiInit);
	}
}

/**
 * @brief	Sends the START signal
 * @param	None
 * @retval	None
 */
void TWI_Start() {
  /*
    The application writes the TWSTA bit to one when it desires to become a Master on the 2-wire Serial Bus.
    The TWI hardware checks if the bus is available, and generates a START condition on the bus if it is free.
    However, if the bus is not free, the TWI waits until a STOP condition is detected, and then generates a 
    new START condition to claim the bus Master status.
    TWSTA must be cleared by software when the START condition has been transmitted.
  */
  TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
  
  // TWINT-bit is set to one by hardware when the TWI has finished its current job and expects application software response
  while (!(TWCR & _BV(TWINT)));
}

/**
 * @brief	Sends the STOP signal
 * @param	None
 * @retval	None
 */
void TWI_Stop() {
  // Writing the TWSTO bit to one in Master mode will generate a STOP condition on the 2-wire Serial Bus
  TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
}

/**
 * @brief	Reads data and ends with an acknowledge
 * @param	None
 * @retval	The data read
 */
uint8_t TWI_ReadAck() {
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
	
	while (!(TWCR & _BV(TWINT)));
	return TWDR;
}

/**
 * @brief	Reads data and doesn't end with an acknowledge
 * @param	None
 * @retval	The data read
 */
uint8_t TWI_ReadNack() {
	TWCR = _BV(TWINT) | _BV(TWEN);
	
	while (!(TWCR & _BV(TWINT)));
	return TWDR;
}

/**
 * @brief	Get the status of the TWI
 * @param	None
 * @retval	The status
 */
uint8_t TWI_GetStatus() {
	// Mask status
	return TWSR & 0xF8;
}

/**
 * @brief	Write data on the bus, could be data or an address. Se TWI_Write for only data
 * @param	Data: The data to write
 * @retval	None
 */
void TWI_WriteRaw(const uint8_t Data) {
	TWDR = Data;
	TWCR = _BV(TWINT) | _BV(TWEN);
	
	while (!(TWCR & _BV(TWINT)));
}

/**
 * @brief	Start transmission to a slave
 * @param	Address: Address to slave which transmission should be started with
 * @retval	None
 */
uint8_t TWI_BeginTransmission(const uint8_t Address)
{
	TWI_Start();
	if (TWI_GetStatus() != TW_START) return 0; // Check if a START condition has been transmitted
	TWI_WriteRaw(Address << 1); // Send the address + Write bit (0)
	if (TWI_GetStatus() != TW_MT_SLA_ACK) return 0; // Check if SLA+W has been transmitted
	return 1;
}

/**
 * @brief	End transmission on the bus
 * @param	None
 * @retval	None
 */
void TWI_EndTransmission()
{
	TWI_Stop();
}

/**
 * @brief	Write real data to the bus
 * @param	Data: The data to write
 * @retval	0: Data didn't transmit
 * @retval	1: Data was transmitted
 */
uint8_t TWI_Write(const uint8_t Data)
{
	TWI_WriteRaw(Data);
	// Check if Data byte has been transmitted
	if (TWI_GetStatus() != TW_MT_DATA_ACK) return 0;
	return 1;
}

/**
 * @brief	Requests a given amount of data from a slave
 * @param	Address: The address to the slave to request from
 * @param	Storage: Pointer to where the data should be stored
 * @param	NumByteToRead: The amount of bytes to receive
 * @retval	10: START condition not sent
 * @retval	20: Slave address + write bit not sent
 * @retval	1: All data received
 */
uint8_t TWI_RequestFrom(const uint8_t Address, uint8_t* Storage, const uint8_t NumByteToRead)
{
	TWI_Start();
	if (TWI_GetStatus() != TW_START) return 10; // Check if a START condition has been transmitted
	TWI_WriteRaw(Address << 1 | 0x1); // Send the address + Read bit (1)
	if (TWI_GetStatus() != TW_MR_SLA_ACK) return 20; // Check if SLA+W has been transmitted
	
	for (uint8_t i = 0; i < NumByteToRead - 1; i++)
		Storage[i] = TWI_ReadAck();
	Storage[NumByteToRead - 1] = TWI_ReadNack();
	
	TWI_Stop();
	return 1;
}

/**
 * @brief	Checks to see if there is a slave at a given address. This is done by beginning a transmission
 *			and see if a slave is responding with an ACK or not
 * @param	Address: The address to look for a slave at
 * @retval	1: There is a slave at the address [Address]
 * @retval	0: There is no slave there
 */
uint8_t TWI_SlaveAtAddress(const uint8_t Address)
{
	uint8_t slaveAvailable = TWI_BeginTransmission(Address);
	TWI_EndTransmission();
	
	return slaveAvailable;
}

/**
 * @brief	Checks to see if TWI has been initialized
 * @param	None
 * @retval	1 if initialized
 * @retval	0 if uninitialized
 */
uint8_t TWI_Initialized()
{
	return _twiInitStatus;
}