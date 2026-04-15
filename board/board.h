/*
 * board.h
 *
 *  Created on: 7 de abr. de 2026
 *      Author: rafael.feijo
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

/*
 * Macros
 */

#define loop						while(1)

#define BoardAssert(x)				assert(x)

#define BOARD_MAXIMUM_CARDS			10

#define	BOARD_TASK_PRIO_LOWEST		(1)
#define	BOARD_TASK_PRIO_LOW			(2)
#define	BOARD_TASK_PRIO_MID			(3)
#define	BOARD_TASK_PRIO_HIGH		(4)
#define	BOARD_TASK_PRIO_HIGHEST		(5)

/*
 * Publics
 */

void board_start();

void board_i2c_lock();

void board_i2c_unlock();

void board_spi_lock();

void board_spi_unlock();

void board_clrc663_powerdown();

void board_clrc663_powerup();

void board_clrc663_deselect();

void board_clrc663_select();

void board_clrc663_txrx(uint8_t *tx, uint8_t *rx, uint32_t len);

void board_eeprom_read(uint32_t i2cAddress, uint32_t memAddress, uint8_t memAddrSize, uint8_t *data, uint32_t len);

void board_eeprom_write(uint32_t i2cAddress, uint32_t memAddress, uint8_t memAddrSize, uint8_t *data, uint32_t len);

void board_led_set();

void board_led_reset();

/* Callback */
void board_gpio_callback(uint32_t GPIO_Pin);

#endif /* BOARD_H_ */
