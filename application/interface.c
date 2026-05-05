/*
 * interface.c
 *
 *  Created on: 7 de abr. de 2026
 *      Author: rafael.feijo
 */

#include "interface.h"

/*
 * enums
 */

typedef enum{
	_EVT_BUTTON,
	_EVT_MESSAGE_BOX,
	_EVT_UPDATE
}_events_e;

/**
 * Structs
 */

typedef struct{
	char *Header;
	char **Options;
	uint32_t OptionsQtd;
}_build_menu_t;

//eventos, ex, pressionar botão, mostrar imagem...

typedef struct{
	_events_e e;

	//only used for message box
	union{
		struct{
			char Text[64];
			uint32_t timeout;
		}msg;
		struct{
			interface_button_e Pressed;
		}btn;
	};
}_queue_evts_t;

/**
 * Variables
 */

static char *MainMenuHeader = "Main Menu";
static char *MainMenuOptions[] = { //vetor de ponteiro
		"Add Card",
		"Delete Card",
		"Delete All"
};

static char *AddCardHeader = "Register Card";
static char *AddCardOptions[] = {
		"Cancel"
};

static char *DelCardHeader = "Select Card";
static char *DelCardOptions[] = {
		"Cancel",
		"Loading..."
};
// used to store the list of the Cards
static char DelCardListOptions[16][30];
static char *pDelCardListOptions[16];

static char *DelAllCardHeader = "Delete All";
static char *DelAllCardOptions[] = {
		"Cancel",
		"Confirm"
};

/** sdd1306 i2c dependent wraps **/

void _wrap_ssd1306_init(){
	board_i2c_lock();
	ssd1306_Init();
	board_i2c_unlock();
}

void _wrap_ssd1306_update(){
	board_i2c_lock();
	ssd1306_UpdateScreen();
	board_i2c_unlock();
}

/**
 * Auxiliary
 */
//Logica para centralização das linhas e para dividir (enter)
void _strcpy_next_line(char *Output, char *Input, uint8_t cont){
	uint8_t i, j, k;

	i=0;
	k=0;
	while (k<cont){
		if (Input[i] == '\n'){
			k++;
		}
		if (Input[i] == '\0'){
			return;
		}
		i++;
	}
	j=0;
	while (Input[i] != '\0' && Input[i] != '\n'){
		Output[j] = Input[i];
		j++;
		i++;
	}
	Output[j] = '\0';
}

void _welcome_screen(){
	ssd1306_DrawRectangle(0, 0, 126, 63, White);
	ssd1306_DrawRectangle(2, 2, 124, 61, White);
	ssd1306_SetCursor(4, 5);
	ssd1306_WriteString("Final Project", Font_16x15, White);
	ssd1306_SetCursor(10, 30);
	ssd1306_WriteString("FreeRTOS", Font_7x10, White);
	ssd1306_SetCursor(10, 42);
	ssd1306_WriteString("Essentials", Font_7x10, White);
	_wrap_ssd1306_update();
}

void _clear_display(){
	ssd1306_Fill(Black);
	ssd1306_DrawRectangle(0, 0, 126, 63, White);
}

void _build_header(char *Text){
	ssd1306_DrawRectangle(0, 0, 126, 14, White);
	ssd1306_SetCursor(3, 3);
	ssd1306_WriteString(Text, Font_7x10, White);
}

//menu de selecoes
void _build_scr (interface_t *interface, _build_menu_t *Menu){
	uint32_t i, j;
	uint32_t elems;
	uint8_t pos;
	char Text[32], pre;

	_clear_display();
	_build_header(Menu->Header); //apresenta retangulo preto

	elems = Menu->OptionsQtd;
	pos = 18;
	if (elems > 0 && Menu->Options != NULL){
		if (elems > 4){
			if (interface->u32CursorIndex > 2){
				i = interface->u32CursorIndex - 2;
			}
			else if (interface->u32CursorIndex > (elems-2)){
				i = elems - 4;
			}
			else{
				i = 0;
			}
		}
		else{
			i = 0;
		}
		j = 0;
		while (j<4 && i<elems){
			if (interface->u32CursorIndex == i){
				pre = '>';
			}
			else{
				pre = ' ';
			}
			sprintf(Text, "%c%s", pre, Menu->Options[i]);
			ssd1306_SetCursor(3, pos);
			pos += 11;
			ssd1306_WriteString(Text, Font_7x10, White);
			i++;
		}
	}
	_wrap_ssd1306_update();
}

//contruir o menu, de acordo, com a tela selecionada
void _build_screen(interface_t *interface){
	_build_menu_t BuildMenu;

	switch (interface->eSelOption){
	case INTERFACE_SCR_MAIN:
		BuildMenu.Header = MainMenuHeader;
		BuildMenu.Options = MainMenuOptions;
		BuildMenu.OptionsQtd = sizeof(MainMenuOptions)/sizeof(char*);
		_build_scr(interface, &BuildMenu);
		break;
	case INTERFACE_SCR_ADD_CARD:
		BuildMenu.Header = AddCardHeader;
		BuildMenu.Options = AddCardOptions;
		BuildMenu.OptionsQtd = sizeof(AddCardOptions)/sizeof(char*);
		_build_scr(interface, &BuildMenu);
		ssd1306_SetCursor(20, 30);
		ssd1306_WriteString("Place card", Font_7x10, White);
		ssd1306_SetCursor(23, 41);
		ssd1306_WriteString("on sensor", Font_7x10, White);
		_wrap_ssd1306_update();
		break;
	case INTERFACE_SCR_DEL_CARD:
		BuildMenu.Header = DelCardHeader;
		if (interface->pu64ListCards == NULL){
			BuildMenu.Options = DelCardOptions;
			BuildMenu.OptionsQtd = sizeof(DelCardOptions)/sizeof(char*);
		}
		else{
			uint32_t i;
			i = 0;
			strcpy(DelCardListOptions[0], "Cancel");
			pDelCardListOptions[0] = DelCardListOptions[0];
			while (i<16 && i<interface->u32ListCardsQtd){
				sprintf(DelCardListOptions[i+1], "%08x", interface->pu64ListCards[i]);
				pDelCardListOptions[i+1] = DelCardListOptions[i+1];
				i++;
			}
			BuildMenu.Options = pDelCardListOptions;
			BuildMenu.OptionsQtd = i+1;
		}
		_build_scr(interface, &BuildMenu);
		break;
	case INTERFACE_SCR_DEL_ALL_CARD:
		BuildMenu.Header = DelAllCardHeader;
		BuildMenu.Options = DelAllCardOptions;
		BuildMenu.OptionsQtd = sizeof(DelAllCardOptions)/sizeof(char*);
		_build_scr(interface, &BuildMenu);
		break;
	}
	interface->u32MaxIndex = BuildMenu.OptionsQtd;
}

//Processa menu de botões, de acordo, com a tecla selecionada
void _process_button(interface_t *interface, interface_button_e e){
	uint32_t optQtd;
	uint32_t CardIndex;

	optQtd = interface->u32MaxIndex;
	switch (interface->eSelOption){
	case INTERFACE_SCR_MAIN:
		if (e == BUTTON_SEL){
			interface->u32CursorIndex++;
			if (interface->u32CursorIndex >= optQtd){
				interface->u32CursorIndex = 0;
			}
		}
		else if (e == BUTTON_ENTER){
			switch (interface->u32CursorIndex){
			case 0:
				interface->eSelOption = INTERFACE_SCR_ADD_CARD;
				interface_cb_event(interface, INTERFACE_EVT_ADD_CARD, NULL);
				break;
			case 1:
				interface->eSelOption = INTERFACE_SCR_DEL_CARD;
				interface->pu64ListCards = NULL;
				interface_cb_event(interface, INTERFACE_EVT_LIST_CARDS, NULL);
				break;
			case 2:
				interface->eSelOption = INTERFACE_SCR_DEL_ALL_CARD;
				break;
			}
			interface->u32CursorIndex = 0;
		}
		break;
	case INTERFACE_SCR_ADD_CARD:
		if (e == BUTTON_SEL){
			interface->u32CursorIndex++;
			if (interface->u32CursorIndex >= optQtd){
				interface->u32CursorIndex = 0;
			}
		}
		else if (e == BUTTON_ENTER){
			switch (interface->u32CursorIndex){
			case 0:
				interface_cb_event(interface, INTERFACE_EVT_CANCEL, NULL);
				interface->eSelOption = INTERFACE_SCR_MAIN;
				break;
			}
			interface->u32CursorIndex = 0;
		}
		break;
	case INTERFACE_SCR_DEL_CARD:
		if (e == BUTTON_SEL){
			interface->u32CursorIndex++;
			if (interface->u32CursorIndex >= optQtd){
				interface->u32CursorIndex = 0;
			}
		}
		else if (e == BUTTON_ENTER){
			switch (interface->u32CursorIndex){
			case 0:
				interface->eSelOption = INTERFACE_SCR_MAIN;
				break;
			default:
				CardIndex = interface->u32CursorIndex - 1;
				interface_cb_event(interface, INTERFACE_EVT_DEL_CARD, &CardIndex);
				break;
			}
			interface->u32CursorIndex = 0;
		}
		break;
	case INTERFACE_SCR_DEL_ALL_CARD:
		if (e == BUTTON_SEL){
			interface->u32CursorIndex++;
			if (interface->u32CursorIndex >= optQtd){
				interface->u32CursorIndex = 0;
			}
		}
		else if (e == BUTTON_ENTER){
			switch (interface->u32CursorIndex){
			case 0:
				interface->eSelOption = INTERFACE_SCR_MAIN;
				interface->u32CursorIndex = 0;
				break;
			case 1:
				interface_cb_event(interface, INTERFACE_EVT_DEL_ALL_CARDS, NULL);
				break;
			}
		}
		break;
	}
}

void _process_message_box(interface_t *interface, char *Text, uint32_t Timeout){
	interface->bIgnoreButtons = true; //ignorar entrada botões durante amostragem de mensagens
	char partText[32];
	uint8_t i, Lines;
	int32_t StartX, StartY;

	_clear_display();
	_build_header("   ! MESSAGE !");

	Lines = 1;
	for (i=0 ; i<strlen(Text) ; i++){
		if (Text[i] == '\n'){
			Lines++;
		}
	}
	if (Lines > 4){
		Lines = 4;
	}
	StartY = (SSD1306_HEIGHT/2 + 5) - (Lines*(Font_7x10.height+1)/2);

	i = 0;
	while (i<Lines){
		_strcpy_next_line(partText, Text, i);
		StartX = (SSD1306_WIDTH/2) - (strlen(partText)*(Font_7x10.width+1)/2);
		// If text is too large, we get a negative start point
		if (StartX >= 0){
			ssd1306_SetCursor(StartX, StartY);
			ssd1306_WriteString(partText, Font_7x10, White);
			StartY += Font_7x10.height+1;
		}
		i++;
	}

	_wrap_ssd1306_update();
	vTaskDelay(pdMS_TO_TICKS(Timeout));

	interface->bIgnoreButtons = false;
}

/**
 * Tasks
 */

void _task_init(interface_t *interface)
{
	 _wrap_ssd1306_init();
	 _welcome_screen();

	 vTaskDelay(pdMS_TO_TICKS(1000));
	 interface->eSelOption = INTERFACE_SCR_MAIN;
	 interface->u32CursorIndex = 0;
	 interface->bIgnoreButtons = false;
	 _build_screen(interface);
}

void _task_interface(void *pvParams)
{
	_queue_evts_t EvtMessage;
	interface_t *interface = (interface_t*)pvParams;

	_task_init(interface);
	loop{
		xQueueReceive(interface->xQueue, &EvtMessage, portMAX_DELAY);


	}
}

/**
 * Publics
 */

void interface_start(interface_t *interface)
{
	BaseType_t xErr;
	BoardAssert(interface != NULL);

	interface->xQueue = xQueueCreate(5, sizeof(_queue_evts_t));
	BoardAssert(interface->xQueue != NULL);
	xErr = xTaskCreate(_task_interface,
			"Task Interface",
			256,
			(void*) (interface),
			BOARD_TASK_PRIO_LOWEST,
			&interface->xTask);
	BoardAssert(xErr == pdPASS);
}

void interface_button_pressed(interface_t *interface, interface_button_e eBtn)
{
	 _queue_evts_t Evt;

	 BoardAssert(interface != NULL);
	 BoardAssert(interface->xQueue != NULL);

	 if (interface->bIgnoreButtons == true)
	 {
		 return;
	 }
	 Evt.e = _EVT_BUTTON;
	 Evt.btn.Pressed = eBtn;

	 xQueueSendFromISR(interface->xQueue, &Evt, NULL);
}

void interface_show_message(interface_t *interface, char* Message, uint32_t TimeMs)
{
	 _queue_evts_t Evt;

	 BoardAssert(interface != NULL);
	 BoardAssert(Message != NULL);
	 BoardAssert(interface->xQueue != NULL);

	 Evt.e = _EVT_MESSAGE_BOX;
	 strcpy(Evt.msg.Text, Message);
	 Evt.msg.timeout = TimeMs;

	 xQueueSendFromISR(interface->xQueue, &Evt, NULL);
}

void interface_send_card_list(interface_t *interface, uint64_t *CardList, uint32_t n)
{
	_queue_evts_t Evt;

	BoardAssert(interface != NULL);
	BoardAssert(CardList != NULL);

	Evt.e = _EVT_UPDATE;
	interface->pu64ListCards = CardList;
	interface->u32ListCardsQtd = n;

	xQueueSendFromISR(interface->xQueue, &Evt, NULL);
}

void interface_set_screen(interface_t* interface, interface_screen_e Scr)
{
	_queue_evts_t Evt;

	BoardAssert(interface != NULL);

	interface->eSelOption = Scr;
	interface->u32CursorIndex = 0;
	Evt.e = _EVT_UPDATE;

	xQueueSendFromISR(interface->xQueue, &Evt, NULL);
}

interface_screen_e interface_get_screen(interface_t* interface)
{
	BoardAssert(interface != NULL);
	return interface->eSelOption;
}


