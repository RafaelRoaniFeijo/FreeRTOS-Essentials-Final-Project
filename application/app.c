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
			xTimerStart(Appl.xTimTimesUp, 0);
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

void _app_cooldown()
{
	Appl.eCardMode = APP_CARD_MODE_COOLDOWN;
	xTimerStart(Appl.xTimCooldown, portMAX_DELAY);
}

void _app_add_card()
{
	Appl.eCardMode = APP_CARD_MODE_REGISTER;
	xTimerStart(Appl.xTimTimesUp, 0);
}

void _app_list_cards()
{
	uint32_t qtd;

	qtd = register_list_cards(&Appl.Register, Appl.CardList);
	interface_send_card_list(&Appl.Interface, Appl.CardList, qtd);
}

void _app_del_card()
{
	register_del_card_by_ID(&Appl.Register, Appl.CardList[Appl.u32DeleteCardIdx]);
	interface_show_message(&Appl.Interface,
			"Selected card\n"
			"removed!",
			1000);
	interface_set_screen(&Appl.Interface, INTERFACE_SCR_MAIN);
}

void _app_del_all_card()
{
	register_del_all_cards(&Appl.Register);
	interface_show_message(&Appl.Interface,
			"All cards\n"
			"deleted!",
			1000);
	interface_set_screen(&Appl.Interface, INTERFACE_SCR_MAIN);
}

void _app_cancel()
{
	Appl.eCardMode = APP_CARD_MODE_READING;		//Volta ao modo de leitura
	xTimerStop(Appl.xTimTimesUp, 0);
}

void _app_detected_card()
{
	char Text [64];
	register_err_e err;

	Appl.u64ReadedCard = rfid_read(&Appl.Rfid);		//recebe o cartao lido

	/*Saber o evento que meu programa esta*/
	if (Appl.eCardMode == APP_CARD_MODE_REGISTER)
	{
		err = register_add_card(&Appl.Register, Appl.u64ReadedCard);

		if (err == REGISTER_OK)
		{
			sprintf(Text,
					"Card Saved!\n"
					"Card Code:\n"
					"%08x",
					Appl.u64ReadedCard);
			interface_show_message(&Appl.Interface, Text, 2000);
		}

		else if (err == REGISTER_NO_FREE_SPACES)
		{
			interface_show_message(&Appl.Interface,
					"Memory is full!",
					2000);
		}

		else if (err == REGISTER_CARD_ALREADY_EXISTS)
		{
			sprintf(Text,
					"Card Already\n"
					"registered!\n"
					"Card Code:\n"
					"%08x", Appl.u64ReadedCard);
			interface_show_message(&Appl.Interface, Text, 2000);
		}
		_app_cooldown();
		interface_set_screen(&Appl.Interface, INTERFACE_SCR_MAIN);
	}

	else if (Appl.eCardMode == APP_CARD_MODE_READING)
	{
		err = register_check_card(&Appl.Register, Appl.u64ReadedCard, NULL);
		if (err == REGISTER_OK)
		{
			sprintf(Text,
					"Access OK!\n"
					"Card Code:\n"
					"%08x",
					Appl.u64ReadedCard);
			interface_show_message(&Appl.Interface, Text, 2000);
		}

		else
		{
			sprintf(Text,
					"Access DENINED!\n"
					"Card Code:\n"
					"%08x",
					Appl.u64ReadedCard);
			interface_show_message(&Appl.Interface, Text, 2000);
		}

		_app_cooldown();
	}
}
/**
 * Timers
 */

void _timer_cooldown_cb (TimerHandle_t xTimer)
{
	if(Appl.eCardMode == APP_CARD_MODE_COOLDOWN)
	{
		Appl.eCardMode = APP_CARD_MODE_READING;
	}
}

void _timer_times_up(TimerHandle_t xTimer)
{
	if(interface_get_screen(&Appl.Interface) != INTERFACE_SCR_MAIN) 	//SE NÃO ESTIVER NA TELA MAIN
	{
		interface_set_screen(&Appl.Interface, INTERFACE_SCR_MAIN);		//vai voltar para a main
		interface_show_message(&Appl.Interface, "Time's Up", 1500);		//mostra times por 1,5s
		Appl.eCardMode = APP_CARD_MODE_READING;							//volta para o modo de leitura
	}
}

void _timer_led(TimerHandle_t xTimer)
{
	switch (Appl.eCardMode)
	{
		case APP_CARD_MODE_COOLDOWN:
			board_led_set();
			break;

		case APP_CARD_MODE_READING:
			if (Appl.u32LedCnt < 3)
			{
				board_led_set();
			}

			else
			{
				board_led_reset();
			}
			break;

		case APP_CARD_MODE_REGISTER:
			if (Appl.u32LedCnt < 6)
			{
				board_led_set();
			}

			else
			{
				board_led_reset();
			}
			break;
	}

	Appl.u32LedCnt++;
	if (Appl.u32LedCnt > 9)
	{
		Appl.u32LedCnt = 0;
	}
}

/**
 * Tasks
 */

void _task_Application(void* pvParams)
{
	EventBits_t xFlagBits;
	interface_t *Interface = &Appl.Interface;		//facilitar a escrita
	register_t *Register = &Appl.Register;
	rfid_t *Rfid = &Appl.Rfid;

	board_start();

	interface_start(Interface);
	register_start(Register);
	rfid_start(Rfid);

	Appl.eCardMode = APP_CARD_MODE_READING;
	xTimerStart(Appl.xTimLed, 0);

	loop{
		xFlagBits = xEventGroupWaitBits(Appl.xFlags,
				EVENT_ALL,
				pdTRUE,
				pdFALSE,
				portMAX_DELAY);

		if (xFlagBits & EVENT_ADD_CARD)
		{
			// enter on state that card will be added
			_app_add_card();
		}

		if (xFlagBits & EVENT_LIST_CARDS)
		{
			// list cards and delivery to the interface
			_app_list_cards();
		}

		if (xFlagBits & EVENT_DEL_CARD)
		{
			// delete a card based on the index
			_app_del_card();
		}

		if (xFlagBits & EVENT_DEL_ALL_CARDS)
		{
			// delete all cards
			_app_del_all_card();
		}

		if (xFlagBits & EVENT_INTERFACE_CANCEL)
		{
			// cancel a operation
			_app_cancel();
		}

		if (xFlagBits & EVENT_CARD_DETECTEC)
		{
			// a card was detected on the front-end antenna
			_app_detected_card();
		}
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
