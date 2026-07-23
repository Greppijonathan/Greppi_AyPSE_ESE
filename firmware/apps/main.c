#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "board.h"

void app_main(void)
{
    printf("Iniciando sistema\n");
    
    if (board_init() == ESP_OK) {
        while (1) {
            float temp = board_leer_temperatura();
            printf("Temperatura: %.2f C\n", temp);
            
            // Actualizamos la pantalla OLED
            board_mostrar_temperatura_oled(temp);
            
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    } else {
        printf("Error! El hardware no pudo inicializarse!\n");
    }
}