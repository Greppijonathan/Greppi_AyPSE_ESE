/**
 * @file middleware.c
 * @brief Implementación de la capa Middleware Wi-Fi y MQTT
 */

#include "middleware.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "MIDDLEWARE";

// Handler global para el cliente MQTT
static esp_mqtt_client_handle_t mqtt_client = NULL;

esp_err_t MID_Network_WiFi_Init(const char* ssid, const char* password) {
    ESP_LOGI(TAG, "Inicializando Red...");

    //Inicializar 
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    //Inicializar la interfaz de red y  eventos
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    // inicialización del Wi-Fi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // CSSID y Password
    wifi_config_t wifi_config = {0};
    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char *)wifi_config.sta.password, password, sizeof(wifi_config.sta.password) - 1);

    ESP_LOGI(TAG, "Conectando al SSID: %s", ssid);

    // Aplicar configuración 
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Conectar al AP 
    return esp_wifi_connect();
}

esp_err_t MID_MQTT_Init(const char* broker_url) {
    ESP_LOGI(TAG, "Configurando cliente MQTT. Broker: %s", broker_url);

   
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = broker_url,
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    if (mqtt_client == NULL) {
        ESP_LOGE(TAG, "Error al crear el handle de MQTT");
        return ESP_FAIL;
    }

    esp_err_t err = esp_mqtt_client_start(mqtt_client);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error arrancando el cliente MQTT: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "Cliente MQTT iniciado correctamente.");
    return ESP_OK;
}

esp_err_t MID_MQTT_Publish_Data(const char* topic, float temp_ambiente) {
    if (mqtt_client == NULL) {
        ESP_LOGE(TAG, "No se puede publicar: Cliente MQTT no inicializado.");
        return ESP_FAIL;
    }

    // Armamos un JSON solo con la temperatura ambiente
    char payload[64];
    snprintf(payload, sizeof(payload), "{\"temp_ambiente\": %.2f}", temp_ambiente);

    int msg_id = esp_mqtt_client_publish(mqtt_client, topic, payload, 0, 1, 0);
    
    if (msg_id < 0) {
        ESP_LOGE(TAG, "Fallo al publicar");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Publicado en '%s': %s", topic, payload);
    return ESP_OK;
}