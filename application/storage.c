/*
 * storage.c
 *
 *  Created on: 7 de abr. de 2026
 *      Author: rafael.feijo
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

/*
 * Auxiliary
 */

//Aguardar resposta do gatekeeper
storage_err_e _send_cmd_to_gatekeeper(storage_t *storage, _storage_data_t *StoCmd)
{
	_storage_rsp_e StoRsp;
	QueueHandle_t xWaitQueue;

	while(storage->xQueueToGatekeeper == NULL)
	{
		vTaskDelay(pdMS_TO_TICKS(100));
	}

	xWaitQueue = xQueueCreate(1, sizeof(_storage_rsp_e));

	StoCmd->xRsp = xWaitQueue;
	xQueueSend(storage->xQueueToGatekeeper, StoCmd, pdMS_TO_TICKS(200));
	xQueueReceive(xWaitQueue, &StoRsp, portMAX_DELAY);

	vQueueDelete(xWaitQueue);

	if(StoRsp == _RSP_FAILED)
	{
		return STORAGE_FAILED;
	}

	return STORAGE_OK;
}

/**
 * Publics
 */

/* Functions */
/** EEPROM Functions for Library **/
eeprom_e EE_Write(uint32_t i2cAddress, uint32_t memAddress, uint8_t memAddrSize, uint8_t *data, uint32_t len)
{
	if(memAddress >=256)
	{
		i2cAddress |= 0x2;
		memAddress -= 256;
	}
	board_i2c_lock(); 				//lock e unlock somente quando for realmente usar, por conta de delays que tem internamente no driver
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

	board_i2c_lock(); 				//lock e unlock somente quando for realmente usar, por conta de delays que tem internamente no driver
	board_eeprom_read( i2cAddress, memAddress, memAddrSize, data, len);
	board_i2c_unlock();

	return EE_OK;
}

/** Tasks **/

void _task_storage(void *pvParams)
{
	storage_t *storage = (storage_t*)pvParams;
	_storage_data_t StoData;
	_storage_rsp_e StoRsp;
	eeprom_t *Eeprom = &storage->EepromHandle;
	eeprom_e eeRsp;

	Eeprom->DelayMs = vTaskDelay;
	Eeprom->EeReadFxn = EE_Read;
	Eeprom->EeWriteFxn = EE_Write;
	Eeprom->i2cAddress = EEPROM_ADDRESS;
	Eeprom->memAddrSize = 1;
	Eeprom->memSize = EEPROM_SIZE;
	Eeprom->pageSize = PAGE_SIZE;
	eeRsp = eeprom_init(Eeprom);
	BoardAssert(eeRsp == EE_OK);

	storage->xQueueToGatekeeper = xQueueCreate(5, sizeof(_storage_data_t));
	loop
	{
		/*Aguardando o recebimento de comandos*/
		xQueueReceive(storage->xQueueToGatekeeper, &StoData, portMAX_DELAY);

		/*Iniciar a operação na eeprom*/

		switch (StoData.eCmd)
		{
			case _CMD_WRITE:
				eeRsp = eeprom_write(Eeprom,
						StoData.u32Address,
						StoData.pu8Buffer,
						StoData.u32Len);
				if(StoData.xRsp != NULL)
				{
					if (eeRsp == EE_OK)
					{
						StoRsp = _RSP_OK;
					}
					else
					{
						StoRsp = _RSP_FAILED;
					}
					xQueueSend(StoData.xRsp, &StoRsp, 0);
				}
				break;

			case _CMD_READ:
				eeRsp = eeprom_read(Eeprom,
						StoData.u32Address,
						StoData.pu8Buffer,
						StoData.u32Len);
				if(StoData.xRsp != NULL)
				{
					if (eeRsp == EE_OK)
					{
						StoRsp = _RSP_OK;
					}
					else
					{
						StoRsp = _RSP_FAILED;
					}
					xQueueSend(StoData.xRsp, &StoRsp, 0);
				}

				break;
		}
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

storage_err_e storage_write(storage_t *storage, uint32_t Addr, uint8_t *DatToWrite, uint32_t Len)
{
	_storage_data_t StoData;

	BoardAssert(storage != NULL);
	BoardAssert(DatToWrite != NULL);
	BoardAssert(Len <= EEPROM_SIZE);

	StoData.eCmd = _CMD_WRITE; //tbm da para colocar dentro do send to gatekeeper, defario
	StoData.pu8Buffer = DatToWrite;
	StoData.u32Len = Len;
	StoData.u32Address = Addr;
	//TODO
	return _send_cmd_to_gatekeeper(storage, &StoData);

}

storage_err_e storage_read(storage_t *storage, uint32_t Addr, uint8_t *DatToRead, uint32_t Len)
{
	_storage_data_t StoData;

	BoardAssert(storage != NULL);
	BoardAssert(DatToWrite != NULL);
	BoardAssert(Len <= EEPROM_SIZE);

	StoData.eCmd = _CMD_READ;
	StoData.pu8Buffer = DatToRead;
	StoData.u32Len = Len;
	StoData.u32Address = Addr;
	//TODO
	return _send_cmd_to_gatekeeper(storage, &StoData);
}
