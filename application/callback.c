/*
 * callback.c
 *
 *  Created on: 7 de abr. de 2026
 *      Author: rafael.feijo
 */
#include "board.h"




/* Hook do FreeRTOS para quando uma Task estoura a pilha (Stack Overflow) */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    /* Se o código chegar aqui, sua Task estourou a memória!
       No debug do mestrado, coloque um breakpoint aqui. */
    while(1);
}
