/*
 * interface.h
 *
 *  Created on: 7 de abr. de 2026
 *      Author: rafael.feijo
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

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

#include "ssd1306.h"
#include "ssd1306_fonts.h"

/**
 * Enumerates
 */

typedef enum{
	INTERFACE_SCR_MAIN,
	INTERFACE_SCR_ADD_CARD,
	INTERFACE_SCR_DEL_CARD,
	INTERFACE_SCR_DEL_ALL_CARD,
}interface_screen_e;

typedef enum{
	BUTTON_NONE,
	BUTTON_SEL,
	BUTTON_ENTER
}interface_button_e;

typedef enum{
	INTERFACE_EVT_ADD_CARD,
	INTERFACE_EVT_LIST_CARDS,
	INTERFACE_EVT_DEL_CARD,
	INTERFACE_EVT_GET_CARDS,
	INTERFACE_EVT_DEL_ALL_CARDS,
	INTERFACE_EVT_CANCEL
}interface_events_cb_e;

/**
 * Structs and Unions
 */

typedef struct{
	interface_screen_e eSelOption;
	uint32_t u32CursorIndex;
	uint32_t u32MaxIndex;
	bool bIgnoreButtons;

	uint64_t *pu64ListCards;		//vetor para listagem de cartoes
	uint32_t u32ListCardsQtd;

	TaskHandle_t xTask;
	QueueHandle_t xQueue;
}interface_t;

/**
 * Publics
 */

void interface_start(interface_t *interface);

void interface_button_pressed(interface_t *interface, interface_button_e eBtn);

void interface_show_message(interface_t *interface, char* Message, uint32_t TimeMs);

void interface_send_card_list(interface_t *interface, uint64_t *CardList, uint32_t n);

void interface_set_screen(interface_t* interface, interface_screen_e Scr);

interface_screen_e interface_get_screen(interface_t* interface);

/* Callbacks */
void interface_cb_event(interface_t *interface, interface_events_cb_e e, uint32_t *data);

#endif /* INTERFACE_H_ */

