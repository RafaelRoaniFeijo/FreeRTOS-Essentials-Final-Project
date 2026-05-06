/*
 * callback.c
 *
 *  Created on: 7 de abr. de 2026
 *      Author: rafael.feijo
 */
#include "interface.h"
#include "board.h"
#include "app.h"

void board_btn_sel_callback()
{
	//TODO implement handler interface
//	interface_button_pressed(interface, BUTTON_SEL);
}

void board_btn_enter_callback()
{
//	interface_button_pressed(interface, BUTTON_ENTER);
}

/* Hook do FreeRTOS para quando uma Task estoura a pilha (Stack Overflow) */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    /* Se o código chegar aqui, sua Task estourou a memória!
       No debug do mestrado, coloque um breakpoint aqui. */
    while(1);
}
