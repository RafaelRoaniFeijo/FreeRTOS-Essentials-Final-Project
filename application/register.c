/*
 * register.c
 *
 *  Created on: 7 de abr. de 2026
 *      Author: rafael.feijo
 */

/**
 * Includes
 */

#include "register.h"


/**
 * Publics
 */

register_err_e register_start(register_t *Register)
{
	BoardAssert(Register);
	storage_start(&Register->Storage);

}

uint32_t register_free_space(register_t *Register);

register_err_e register_check_card(register_t *Register, uint64_t CardID, uint32_t *IDX);

register_err_e register_add_card(register_t *Register, uint64_t CardID);

uint32_t register_list_cards(register_t *Register, uint64_t *OutArray);

register_err_e register_del_card_by_ID(register_t *Register, uint64_t CardID);

register_err_e register_del_card_by_IDX(register_t *Register, uint32_t IDX);

register_err_e register_del_all_cards(register_t *Register);
