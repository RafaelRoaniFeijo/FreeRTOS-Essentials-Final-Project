/*
 * board.c
 *
 *  Created on: 7 de abr. de 2026
 *      Author: rafael.feijo
 */

/*
 * Includes
 */
#include "board.h"
#include <stm32g4xx.h>
#include "i2c.h"
#include "gpio.h"

/*
 * Privates
 */

/*
 * Macros
 */
#define BOARD_I2C_EEPROM		&hi2c3
#define BOARD_I2C_SSD1306		&hi2c3

#define BOARD_SPI_CLRC663		&hspi1

/*
 * Structs
 */
typedef struct{
	SemaphoreHandle_t xI2Cmutex;
	SemaphoreHandle_t xSPImutex;
}_board_t;

/** Private Globals **/
static _board_t _Board;

/*
 * Hardware Callbacks
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	board_gpio_callback(GPIO_Pin);
}


/*
 * Publics
 */

void board_start()
{
	_Board.xI2Cmutex = xSemaphoreCreateMutex();
	_Board.xSPImutex = xSemaphoreCreateMutex();
	BoardAssert(_Board.xI2Cmutex != NULL);			//se for diferente de null segue, se não fica em loop
	BoardAssert(_Board.xSPImutex != NULL);
}

void board_i2c_lock()
{
	xSemaphoreTake(_Board.xI2Cmutex, portMAX_DELAY);
}

void board_i2c_unlock()
{
	xSemaphoreGive(_Board.xI2Cmutex);
}

void board_spi_lock()
{
	xSemaphoreTake(_Board.xSPImutex, portMAX_DELAY);
}

void board_spi_unlock()
{
	xSemaphoreGive(_Board.xSPImutex);
}

void board_clrc663_powerdown()
{
	HAL_GPIO_WritePin(CLR663_PDOWN_GPIO_Port, CLR663_PDOWN_Pin, GPIO_PIN_SET);
}

void board_clrc663_powerup()
{
	HAL_GPIO_WritePin(CLR663_PDOWN_GPIO_Port, CLR663_PDOWN_Pin, GPIO_PIN_RESET);
}

void board_clrc663_deselect()
{
	HAL_GPIO_WritePin(CLR663_nCS_GPIO_Port, CLR663_nCS_Pin, GPIO_PIN_SET);
}

void board_clrc663_select()
{
	HAL_GPIO_WritePin(CLR663_nCS_GPIO_Port, CLR663_nCS_Pin, GPIO_PIN_RESET);
}

void board_clrc663_txrx(uint8_t *tx, uint8_t *rx, uint32_t len)
{
	HAL_SPI_TransmitReceive(BOARD_SPI_CLRC663, tx, rx, len, 100);
}

void board_eeprom_read(uint32_t i2cAddress, uint32_t memAddress, uint8_t memAddrSize, uint8_t *data, uint32_t len)
{
	HAL_I2C_Mem_Read(BOARD_I2C_EEPROM, i2cAddress, memAddress, memAddrSize, data, len, 100);
}

void board_eeprom_write(uint32_t i2cAddress, uint32_t memAddress, uint8_t memAddrSize, uint8_t *data, uint32_t len)
{
	HAL_I2C_Mem_Write(BOARD_I2C_EEPROM, i2cAddress, memAddress, memAddrSize, data, len, 100);
}

void board_led_set()
{
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}

void board_led_reset()
{
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}


