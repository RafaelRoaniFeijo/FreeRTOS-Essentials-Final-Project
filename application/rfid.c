/*
 * rfid.c
 *
 *  Created on: 28 de abr. de 2026
 *      Author: rafael.feijo
 */


#include "rfid.h"

/**
 * Macros
 */

#define _DELAY_WHEN_DETECT		1500	//Delay quando detectado
#define _DELAY_WHEN_NOT_DETECT	300		//LEITURA A CADA 300 ms

/* Enumerates */

typedef enum{
	_RFID_CARD_NOT_PRESENT,
	_RFID_CARD_DETECTED
}_rfid_card_det_e;

/** Externs from MFRC630 driver **/
void mfrc630_SPI_transfer(uint8_t* tx, uint8_t* rx, uint16_t len)
{
	board_clrc663_txrx(tx, rx, (uint32_t)len);
}

void mfrc630_SPI_select()
{
	board_spi_lock();				//Bloqueia SPI
	board_clrc663_select();			//Depois ativa o CS
}

void mfrc630_SPI_unselect()
{
	board_clrc663_deselect();		//Deselect no CS
	board_spi_unlock();				//Libera o mutex
}

/* RFID Functions */

void _rfid_init(rfid_t *Rfid)
{

	// Reset the CLRC663
	board_clrc663_powerdown();
	vTaskDelay(pdMS_TO_TICKS(10));
	board_clrc663_powerup();
	vTaskDelay(pdMS_TO_TICKS(10));

	// Here, we only use the pooling mode
	// A better approach is to use the LPCD Mode
	Rfid->u8VersionReg = mfrc630_read_reg(MFRC630_REG_VERSION);
	mfrc630_AN1102_recommended_registers(MFRC630_PROTO_ISO14443A_106_MILLER_MANCHESTER);
}

_rfid_card_det_e _rfid_pool_detect(rfid_t *Rfid)
{
	uint16_t ret;
	uint64_t uid = 0;
	uint8_t sak, uid_len;

	ret = mfrc630_iso14443a_REQA();
	if(ret != 0) 		//Se diferente de 0, significa que tem uma tag prox
	{
		uid_len = mfrc630_iso14443a_select((uint8_t *)uid, &sak); 	//entao faz a leitura da uid
		Rfid->u8TagUidLen = uid_len;
		if (uid_len > 0)			// se maior que 0, conseguiu ler a tag
		{
			Rfid->u64TagUID = uid;

			return _RFID_CARD_DETECTED;
		}
		else
		{
			return _RFID_CARD_NOT_PRESENT;
		}
	}
}

/* Tasks */

/**
 * Publics
 */

void rfid_start(rfid_t *Rfid)
{

}

uins64_t rfid_read(rfid_t *Rfid)
{
	uint64_t u64TagId;

	BoardAssert(Rfid != NULL);

	u64TagId = Rfid->u64TagUID;
	Rfid->u64TagUID = 0;

	return u64TagId;
}

/* Callback */
void __WEAK rfid_card_detected(rfid_t *Rfid)
{
	(void)Rfid;
}
