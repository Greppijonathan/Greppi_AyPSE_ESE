#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "board.h"

void app_main(void)
{
    printf("¡Iniciando Firmware del Proyecto AyPSE!\n");
    
    if (board_init() == ESP_OK) {
        while (1) {
            float temp = board_leer_temperatura();
            printf("Temperatura actual: %.2f C\n", temp);
            
            // Actualizamos la pantalla OLED
            board_mostrar_temperatura_oled(temp);
            
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    } else {
        printf("Error crítico: el hardware no pudo inicializarse.\n");
    }
}