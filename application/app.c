/*
 * app.c
 *
 *  Created on: 7 de abr. de 2026
 *      Author: rafael.feijo
 */

#include "app.h"

/** Application Global Var **/
app_t Appl = {0};

/**
 * Calbacks
 */

void interface_cb_event(interface_t *interface, interface_events_cb_e e, uint32_t *data)
{
	switch (e)
	{
		case INTERFACE_EVT_ADD_CARDS:
			xEventGroupSetBits(Appl.xFlags, EVENT_ADD_CARD);
			//TODO Initialize tim's up timer
			break;

		case INTERFACE_EVT_LIST_CARDS:
			xEventGroupSetBits(Appl.xFlags, EVENT_LIST_CARDS);
			break;

		case INTERFACE_EVT_DEL_CARDS:
			xEventGroupSetBits(Appl.xFlags, EVENT_DEL_CARD);
			Appl.u32DeleteCardIdx = *data;
			break;

		case INTERFACE_EVT_DEL_ALL_CARDS:
			xEventGroupSetBits(Appl.xFlags, EVENT_DEL_ALL_CARDS);
			break;

		case INTERFACE_EVT_CANCEL:
			xEventGroupSetBits(Appl.xFlags, EVENT_INTERFACE_CANCEL);
			//TODO Stop the time's up timer
			break;

		case INTERFACE_EVT_GET_CARDS:
			//nothing to do
			break;
	}
}

void rfid_card_detected(rfid_t *Rfid)
{
	xEventGroupSetBits(Appl.xFlags, EVENT_CARD_DETECTEC);
}

/**
 * Privates
 */

/**
 * Timers
 */

void _timer_cooldown_cb (TimerHandle_t xTimer)
{

}

void _timer_times_up(TimerHandle_t xTimer)
{

}

void _timer_led(TimerHandle_t xTimer)
{

}

/**
 * Tasks
 */

void _task_Application(void* pvParams)
{
	EventBits_t xFlagsBits;
	interface_t *Interface = &Appl.Interface;		//facilitar a escrita
	register_t *Register = &Appl.Register;
	rfid_t *Rfid = &Appl.Rfid;

	board_start();

	interface_start(Interface);
	register_start(Register);
	rfid_start(Rfid);

	Appl.eCardMode = APP_CARD_MODE_READING;

	loop{

	}
}

/**
 * Publics
 */

void app_start()
{
	BaseType_t xErr;

	Appl.xFlags = xEventGroupCreate();
	BoardAssert(Appl.xFlags != NULL);

	Appl.xTimCooldown = xTimerCreate("Cooldown",
			pdMS_TO_TICKS(2500),
			pdFALSE,
			NULL,
			_timer_cooldown_cb);
	BoardAssert(Appl.xTimCooldown != NULL);

	Appl.xTimTimesUp = xTimerCreate("Time's Up",
			pdMS_TO_TICKS(10000),
			pdFALSE,
			NULL,
			_timer_times_up);
	BoardAssert(Appl.xTimTimesUp != NULL);

	Appl.xTimLed = xTimerCreate("Timer Led",
			pdMS_TO_TICKS(100),
			pdTRUE, //timer reload
			NULL,
			_timer_led);
	BoardAssert(Appl.xTimLed != NULL);

	xErr = xTaskCreate(_task_Application,
			"Task Appl",
			256,
			(void*)NULL,
			BOARD_TASK_PRIO_LOWEST,
			&Appl.xTask);
	BoardAssert(xErr == pdPASS);
}
