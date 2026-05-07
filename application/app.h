/*
 * app.h
 *
 *  Created on: 7 de abr. de 2026
 *      Author: rafael.feijo
 */

#ifndef APP_H_
#define APP_H_

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

#include "interface.h"
#include "register.h"
#include "app.h"
#include "rfid.h"

/**
 * Macros
 */


/**
 * Enumerates
 */

typedef enum{
	/* Interface Events */
	EVENT_ADD_CARD			= 0x0001,
	EVENT_LIST_CARDS		= 0x0002,
	EVENT_DEL_CARD			= 0x0004,
	EVENT_DEL_ALL_CARDS		= 0x0008,
	EVENT_CANCEL			= 0x0010,
	EVENT_INTERFACE_CANCEL 	= 0x0020,

	/* RFID Events */
	EVENT_CARD_DETECTEC  	= 0x0040,

	EVENT_ALL 				= 0x00FF
}app_events_e;


typedef enum{
	APP_CARD_MODE_COOLDOWN,
	APP_CARD_MODE_READING,
	APP_CARD_MODE_REGISTER
}app_card_mode_e;

/**
 * Structs and Unions
 */

typedef struct{
	interface_t Interface;
	register_t Register;
	rfid_t Rfid;

	app_card_mode_e eCardMode; //para facilitar o debug

	uint64_t u64ReadedCard;
	uint32_t u32DeleteCardIdx;
	uint64_t CardList[BOARD_MAXIMUM_CARDS];

	uint32_t u32LedCnt;

	TaskHandle_t xTask;
	EventGroupHandle_t xFlags;
	TimerHandle_t xTimCooldown;
	TimerHandle_t xTimTimesUp; //add new card
	TimerHandle_t xTimLed;
}app_t;

/**
 * Externs
 */

extern app_t Appl;	//porque tem o callback

/**
 * Publics
 */

void app_start();

#endif /* APP_H_ */
