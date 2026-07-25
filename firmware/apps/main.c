/**
 * @file main.c
 * @brief Capa de Aplicación del Termostato IoT.
 * @details Orquesta la inicialización de los recursos, gestiona la conectividad a la nube y 
 *          administra el flujo de datos  utilizando tareas y colas de FreeRTOS.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "board.h"
#include "middleware.h"
#include "nvs_flash.h"

/** @brief Nombre de la red Wi-Fi local. */
#define WIFI_SSID       "***"

/** @brief Contraseña de la red Wi-Fi local. */
#define WIFI_PASS       "***"

/** @brief URL del servidor (broker) MQTT público. */
#define MQTT_BROKER_URL "mqtt://broker.hivemq.com"

/** 
 * @brief Cola de FreeRTOS para comunicar la temperatura entre las tareas de lectura y publicación. 
 */
QueueHandle_t temp_queue;

/**
 * @brief Tarea 1: Tarea productora (Alta Prioridad).
 * @details Lee la temperatura desde la capa Board Support, actualiza la interfaz local 
 *          en la pantalla OLED, y sobreescribe la lectura más reciente en la cola.
 * @param[in] pvParameters Parámetros pasados al crear la tarea (no utilizados).
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
        // Si la cola está llena, no bloqueamos (0 ticks) y sobrescribimos el valor anterior
        xQueueOverwrite(temp_queue, &temp); 
        
        // Medimos y actualizamos la pantalla cada 2 segundos
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

/**
 * @brief Tarea 2: Tarea consumidora (Prioridad Media).
 * @details Espera bloqueada hasta recibir un dato en la cola, luego lo publica 
 *          en la nube mediante el broker MQTT y aguarda 5 segundos antes de reanudar.
 * @param[in] pvParameters Parámetros pasados al crear la tarea (no utilizados).
 */
void task_mqtt_publish(void *pvParameters)
{
    float temp_recibida = 0.0;
    while (1) {
        // Esperamos el dato indefinidamente (portMAX_DELAY) sin consumir ciclos de CPU
        if (xQueueReceive(temp_queue, &temp_recibida, portMAX_DELAY) == pdTRUE) {
            
            // Le pasamos directamente el valor al middleware
            MID_MQTT_Publish_Data("greppi/aypse/temp", temp_recibida);
            
            // Cadencia tras el envío
            vTaskDelay(pdMS_TO_TICKS(5000));
        }
    }
}

/**
 * @brief Función principal y punto de entrada de la aplicación de ESP-IDF.
 * @details Inicializa secuencialmente las capas inferiores (Hardware, Wi-Fi, MQTT), 
 *          crea los mecanismos IPC y lanza las tareas de FreeRTOS.
 */
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

    // Tareas de FreeRTOS
    xTaskCreate(task_medir_y_oled, "Task_Medir_OLED", 4096, NULL, 5, NULL);
    xTaskCreate(task_mqtt_publish, "Task_MQTT", 4096, NULL, 4, NULL);
}