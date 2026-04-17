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
	Register->u32TotalSpace = REGISTER_MAXIMUM_CARDS;
	register_cards_registered(Register); 						/*Calcula quanto tem de espaço livre*/

	return REGISTER_OK;
}

uint32_t register_cards_registered(register_t *Register)
{
	uint32_t i, address, qtd;
	storage_t *Storage = &Register->Storage;
	reg_card_t *RegCard = &Register->RegCard;

	BoardAssert(Register);

	qtd = 0;
	address = REGISTER_EE_CARDS_START;
	for (i = 0; i < REGISTER_MAXIMUM_CARDS; ++i)   /*Conta quantos cartões possuí*/
	{
		storage_read(Storage, address, (uint8_t*)RegCard, sizeof(reg_card_t));
		if (RegCard->u8UsedMask == REGISTER_USED_MASK)
		{
			qtd++;
		}
		address += REGISTER_EE_CARDS_STEP;
	}
	Register->u32CardsRegistered = qtd;
	Register->u32FreeSpace = Register->u32TotalSpace - Register->u32CardsRegistered;

	return qtd;			/*Retorna quantos cartoes tem cadastrado*/
}

register_err_e register_check_card(register_t *Register, uint64_t CardID, uint32_t *IDX);

register_err_e register_add_card(register_t *Register, uint64_t CardID);

uint32_t register_list_cards(register_t *Register, uint64_t *OutArray);

register_err_e register_del_card_by_ID(register_t *Register, uint64_t CardID);

register_err_e register_del_card_by_IDX(register_t *Register, uint32_t IDX);

register_err_e register_del_all_cards(register_t *Register);
