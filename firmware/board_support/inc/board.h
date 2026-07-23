#ifndef BOARD_H
#define BOARD_H

#include "esp_err.h"

/**
 * @brief Inicializa los periféricos de la placa 
 */
esp_err_t board_init(void);

/**
 * @brief Lee la temperatura del objeto desde el sensor MLX90614
 */
float board_leer_temperatura(void);

/**
 * @brief Muestra la temperatura en la pantalla OLED 
 * @param temp Valor flotante de la temperatura a renderizar
 */
void board_mostrar_temperatura_oled(float temp);

#endif // BOARD_H