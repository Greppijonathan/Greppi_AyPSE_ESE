/**
 * @file board.h
 * @brief Capa de Soporte de Placa (Board Support).
 * @details Expone las funciones de alto nivel para inicializar el hardware, 
 *          abstraer la lectura del sensor térmico y el uso de la pantalla OLED.
 */

#ifndef BOARD_H
#define BOARD_H

#include "esp_err.h"

/**
 * @brief Inicializa los periféricos de la placa.
 * @details Configura el bus I2C mediante la HAL, inicializa el sensor MLX90614 y arranca la pantalla OLED.
 * @return ESP_OK si la inicialización de todos los componentes fue exitosa, 
 *         o un código de error nativo en caso de fallo.
 */
esp_err_t board_init(void);

/**
 * @brief Lee la temperatura ambiente desde el sensor MLX90614.
 * @return El valor de la temperatura en grados Celsius. Devuelve -999.0 en caso de error de lectura.
 */
float board_leer_temperatura(void);

/**
 * @brief Muestra la temperatura en la pantalla OLED.
 * @details Formatea el valor flotante y lo renderiza en el display limpiando el buffer previo.
 *          Si la temperatura indica un error (<= -900.0), muestra un mensaje de "ERROR".
 * @param[in] temp Valor flotante de la temperatura a renderizar.
 */
void board_mostrar_temperatura_oled(float temp);

#endif // BOARD_H