#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "board.h"
#include "middleware.h"
#include "nvs_flash.h"

#define WIFI_SSID       "Jona_2.4GHz"
#define WIFI_PASS       "35425773"
#define MQTT_BROKER_URL "mqtt://broker.hivemq.com"

// Cola para comunicar la temperatura entre las tareas
QueueHandle_t temp_queue;

/**
 * @brief Tarea 1: Mide temperatura y actualiza el OLED 
 */
void task_medir_y_oled(void *pvParameters)
{
    float temp = 0.0;
    while (1) {
        temp = board_leer_temperatura();
        printf("Sensor: %.2f C\n", temp);
        
        // Actualizamos la pantalla OLED
        board_mostrar_temperatura_oled(temp);
        
        // Enviamos el dato a la cola para la tarea de MQTT
        // Si la cola está llena, no bloqueamos (0 ticks)
        xQueueOverwrite(temp_queue, &temp); 
        
        // Medimos y actualizamos la pantalla cada 2 segundos
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

/**
 * @brief Tarea 2: Publica los datos en MQTT (Prioridad Media)
 */
void task_mqtt_publish(void *pvParameters)
{
    float temp_recibida = 0.0;
    while (1) {
        if (xQueueReceive(temp_queue, &temp_recibida, portMAX_DELAY) == pdTRUE) {
            
            // Le pasamos directamente el valor al middleware
            MID_MQTT_Publish_Data("greppi/aypse/temp", temp_recibida);
            
            vTaskDelay(pdMS_TO_TICKS(5000));
        }
    }
}
void app_main(void)
{
    printf("Iniciando sistema...\n");
    
    // Inicializar el hardware
    if (board_init() != ESP_OK) {
        printf("Error! El hardware no pudo inicializarse!\n");
        return; // Detenemos la ejecución si falla el hardware
    }

    // Inicializar Middleware (Wi-Fi y MQTT)
    MID_Network_WiFi_Init(WIFI_SSID, WIFI_PASS);
    
    // Margen de unos segundos para que se conecte al Wi-Fi
    vTaskDelay(pdMS_TO_TICKS(5000)); 
    
    MID_MQTT_Init(MQTT_BROKER_URL);

    // Cola (tamaño de 1 elemento, tipo float)
    temp_queue = xQueueCreate(1, sizeof(float));
    if (temp_queue == NULL) {
        printf("Error creando la cola de FreeRTOS\n");
        return;
    }

    //Tareas de FreeRTOS

    xTaskCreate(task_medir_y_oled, "Task_Medir_OLED", 4096, NULL, 5, NULL);
    xTaskCreate(task_mqtt_publish, "Task_MQTT", 4096, NULL, 4, NULL);
}