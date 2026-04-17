/*
 * register.h
 *
 *  Created on: 7 de abr. de 2026
 *      Author: rafael.feijo
 *      Gerenciamento dos cartões de acesso
 */

#ifndef REGISTER_H_
#define REGISTER_H_

/*
 * Includes
 */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <cmsis_gcc.h>

#include "board.h"

#include "storage.h"

/*
 * Macros
 */

#define REGISTER_USED_MASK			0x51  					//indica que aquele registro está ocupado
#define REGISTER_UNUSED_MASK		0xFF					//Posição não está sendo utilizada

#define REGISTER_MAXIMUM_CARDS		BOARD_MAXIMUM_CARDS

#define REGISTER_EE_CARDS_START		0x10					//Inicia no end 10
#define REGISTER_EE_CARDS_STEP		sizeof(reg_card_t)

/**
 * Enumerates
 */

typedef enum
{
	REGISTER_OK,
	REGISTER_NOT_FOUND,
	REGISTER_FAILED,
	REGISTER_NO_FREE_SPACES,
	REGISTER_CARD_ALREADY_EXISTS,
	REGISTER_UNKNOWN = 0xFF
}register_err_e;

/**
 * Structs and Unions
 */

typedef struct
{
	uint8_t u8UsedMask;
	uint64_t u64CardID;
}reg_card_t;

typedef struct
{
	storage_t Storage;			//instanciamento storage
	reg_card_t RegCard;

	uint32_t u32CardsRegistered;
	uint32_t u32FreeSpace;
	uint32_t u32TotalSpace;
}register_t;

/**
 * Publics
 */

register_err_e register_start(register_t *Register);

uint32_t register_cards_registered(register_t *Register);

register_err_e register_check_card(register_t *Register, uint64_t CardID, uint32_t *IDX);

register_err_e register_add_card(register_t *Register, uint64_t CardID);

uint32_t register_list_cards(register_t *Register, uint64_t *OutArray);

register_err_e register_del_card_by_ID(register_t *Register, uint64_t CardID);

register_err_e register_del_card_by_IDX(register_t *Register, uint32_t IDX);

register_err_e register_del_all_cards(register_t *Register);

#endif /* REGISTER_H_ */
