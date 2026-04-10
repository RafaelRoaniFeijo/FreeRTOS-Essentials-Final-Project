/*
 * storage.c
 *
 *  Created on: 7 de abr. de 2026
 *      Author: rafael.feijo
 */

/**
 * Publics
 */

/*
 * Includes
 */
#include "storage.h"

/**
 * Enumerates
 */

typedef enum{
	_CMD_WRITE,
	_CMD_READ
}_storage_cmd_e;

typedef enum{
	_RSP_OK,
	_RSP_FAILED
}_storage_rsp_e;

/*
 * Structs
 */
typedef struct{
	_storage_cmd_e	eCmd;
	uint8_t *pu8Buffer;
	uint32_t u32Len;
	uint32_t u32Address;
	QueueHandle_t xRsp;
}_storage_data_t;

/*
 * Macros
 */

#define EEPROM_SIZE		512
#define PAGE_SIZE		16
#define EEPROM_ADDRESS	0xA0

/* Functions */
/** EEPROM Functions for Library **/
eeprom_e EE_Write(uint32_t i2cAddress, uint32_t memAddress, uint8_t memAddrSize, uint8_t *data, uint32_t len)
{
	if(memAddress >=256)
	{
		i2cAddress |= 0x2;
		memAddress -= 256;
	}
	board_i2c_lock();
	board_eeprom_write(i2cAddress, memAddress, memAddrSize, data, len);
	board_i2c_unlock();

	return EE_OK;
}

eeprom_e EE_Read(uint32_t i2cAddress, uint32_t memAddress, uint8_t memAddrSize, uint8_t *data, uint32_t len)
{
	if (memAddress >= 256)
	{
		i2cAddress |= 0x2;
		memAddress -= 256;
	}

	board_i2c_lock();
	board_eeprom_read( i2cAddress, memAddress, memAddrSize, data, len);
	board_i2c_unlock();

	return EE_OK;
}

/** Tasks **/

void _task_storage(void *pvParams)
{
	loop
	{

	}
}

void storage_start(storage_t *storage)
{
	BaseType_t Err;

	BoardAssert(storage != NULL); 		//SE VIER null trava o programa aqui

	xErr = xTaskCreate(_task_storage,
			"Task storage",
			256,
			(void*)(storage),
			BOARD_TASK_PRIO_MID,
			&storage->xTask); 			//pegar o endereço do membro da estrutura
	BoardAssert(xErr == pdPASS);	 	//se parar aqui é porque está sem memoria heap suficiente

}

storage_err_e storage_write(storage_t *storage, uint32_t Addr, uint8_t *DatToWrite, uint32_t Len);

storage_err_e storage_read(storage_t *storage, uint32_t Addr, uint8_t *DatToRead, uint32_t Len);
