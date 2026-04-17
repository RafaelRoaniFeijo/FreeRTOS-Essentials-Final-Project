/*
 * storage.h
 *
 *  Created on: 7 de abr. de 2026
 *      Author: rafael.feijo
 */

#ifndef STORAGE_H_
#define STORAGE_H_

/**
 * Includes
 */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <cmsis_gcc.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <event_groups.h>
#include <semphr.h>
#include <timers.h>

#include "board.h"

#include "eeprom.h"

/**
 * Enumerates
 */
typedef enum{
	STORAGE_OK,
	STORAGE_FAILED
}storage_err_e;


/**
 * Structs and Unions
 */

typedef struct{
	QueueHandle_t 	xQueueToGatekeeper;
	TaskHandle_t	xTask;
	eeprom_t		EepromHandle;
}storage_t;

/**
 * Publics
 */

void storage_start(storage_t *storage);

storage_err_e storage_write(storage_t *storage, uint32_t Addr, uint8_t *DatToWrite, uint32_t Len);

storage_err_e storage_read(storage_t *storage, uint32_t Addr, uint8_t *DatToRead, uint32_t Len);

#endif /* STORAGE_H_ */
