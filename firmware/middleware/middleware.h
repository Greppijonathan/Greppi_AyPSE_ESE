#ifndef MIDDLEWARE_H
#define MIDDLEWARE_H

#include "esp_err.h"

// Inicializa la conexión Wi-Fi 
esp_err_t MID_Network_WiFi_Init(const char* ssid, const char* password);

// Inicializa el cliente MQTT y se conecta al broker
esp_err_t MID_MQTT_Init(const char* broker_url);

// Formatea las temperaturas a JSON y las publica en un tópico
esp_err_t MID_MQTT_Publish_Data(const char* topic, float temp_ambiente);

#endif // MIDDLEWARE_H