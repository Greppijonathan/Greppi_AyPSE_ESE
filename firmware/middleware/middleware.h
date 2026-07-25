/**
 * @file middleware.h
 * @brief Capa de Middleware para conectividad Wi-Fi y MQTT.
 * @details Provee servicios de infraestructura y comunicación estándar para la aplicación,
 *          encapsulando la pila de red y el cliente de mensajería.
 */

#ifndef MIDDLEWARE_H
#define MIDDLEWARE_H

#include "esp_err.h"

/**
 * @brief Inicializa la conexión Wi-Fi en modo Estación (STA).
 * @details Configura la memoria NVS, inicializa la interfaz de red LwIP y el bucle de eventos,
 *          y conecta el dispositivo al punto de acceso especificado.
 * @param[in] ssid Nombre de la red Wi-Fi a la que conectarse.
 * @param[in] password Contraseña de la red Wi-Fi.
 * @return ESP_OK si la configuración y orden de conexión fueron exitosas, o un código de error en caso de fallo.
 */
esp_err_t MID_Network_WiFi_Init(const char* ssid, const char* password);

/**
 * @brief Inicializa el cliente MQTT y establece la conexión con el broker.
 * @details Crea la tarea en segundo plano para gestionar ping-req, keep-alive y 
 *          reconexiones automáticas.
 * @param[in] broker_url Cadena con la URL o dirección IP del broker MQTT (ej. "mqtt://broker.hivemq.com").
 * @return ESP_OK si el cliente se inicializó y arrancó correctamente, o un código de error nativo.
 */
esp_err_t MID_MQTT_Init(const char* broker_url);

/**
 * @brief Formatea la temperatura a JSON y la publica en un tópico MQTT.
 * @details Empaqueta el valor numérico dentro de un string con formato JSON y lo encola 
 *          en el cliente MQTT utilizando Calidad de Servicio (QoS) 1.
 * @param[in] topic Cadena de texto con el tópico MQTT de destino.
 * @param[in] temp_ambiente Valor flotante de la temperatura ambiente a publicar.
 * @return ESP_OK si el mensaje se encoló correctamente, o un código de error nativo.
 */
esp_err_t MID_MQTT_Publish_Data(const char* topic, float temp_ambiente);

#endif // MIDDLEWARE_H