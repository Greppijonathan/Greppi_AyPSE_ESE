/**
 * @file i2c_driver.h
 * @brief Capa de Abstracción de Hardware (HAL) para bus I2C.
 * @details Provee las funciones esenciales de comunicación I2C encapsulando el driver nativo de ESP-IDF 
 *          para garantizar un bajo acoplamiento con las capas de hardware y soporte de placa.
 */

#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include <stdint.h>
#include <stddef.h>
#include "driver/i2c_master.h"
#include "esp_err.h"

/** @brief Pin GPIO asignado a la línea de datos (SDA). */
#define I2C_MAESTRO_SDA_IO           6      

/** @brief Pin GPIO asignado a la línea de reloj (SCL). */
#define I2C_MAESTRO_SCL_IO           7      

/** @brief Frecuencia de reloj del bus I2C en Hz (100 kHz - Modo Estándar). */
#define I2C_MAESTRO_FREQ_HZ          100000 

/** @brief Número de puerto I2C de hardware utilizado. */
#define I2C_MAESTRO_NUM              I2C_NUM_0

/**
 * @brief Inicializa el bus I2C maestro.
 * @param[out] manejador_bus Puntero al manejador del bus I2C a crear.
 * @return ESP_OK si la inicialización fue exitosa.
 */
esp_err_t i2c_driver_inicializar(i2c_master_bus_handle_t *manejador_bus);

/**
 * @brief Libera los recursos del bus I2C maestro.
 * @param[in] manejador_bus Manejador del bus I2C a desinicializar.
 * @return ESP_OK si se liberó correctamente.
 */
esp_err_t i2c_driver_desinicializar(i2c_master_bus_handle_t manejador_bus);

/**
 * @brief Verifica si un dispositivo responde en la dirección dada (ACK/NACK).
 * @param[in] manejador_bus Manejador del bus I2C.
 * @param[in] direccion_dev Dirección I2C de 7 bits del dispositivo.
 * @return ESP_OK si responde ACK, o código de error si no responde.
 */
esp_err_t i2c_driver_probar_dispositivo(i2c_master_bus_handle_t manejador_bus, uint16_t direccion_dev);

/**
 * @brief Escribe un buffer de datos directamente a un dispositivo I2C (sin registro explícito).
 * 
 * @param[in] manejador_bus Manejador del bus I2C.
 * @param[in] direccion_dev Dirección I2C de 7 bits del dispositivo esclavo.
 * @param[in] datos Buffer con los bytes a transmitir.
 * @param[in] longitud Cantidad de bytes en el buffer.
 * @return ESP_OK si la escritura fue exitosa.
 */
esp_err_t i2c_driver_escribir(i2c_master_bus_handle_t manejador_bus, 
                             uint8_t direccion_dev, 
                             const uint8_t *datos, 
                             size_t longitud);

/**
 * @brief Lee datos desde un registro específico de un dispositivo I2C.
 * 
 * @param[in] manejador_bus Manejador del bus I2C.
 * @param[in] direccion_dev Dirección I2C del dispositivo esclavo.
 * @param[in] registro Comando o dirección del registro a leer.
 * @param[out] datos Buffer donde se guardarán los datos leídos.
 * @param[in] longitud Cantidad de bytes a leer.
 * @return ESP_OK si la lectura fue exitosa.
 */
esp_err_t i2c_driver_leer_registro(i2c_master_bus_handle_t manejador_bus, 
                                   uint8_t direccion_dev, 
                                   uint8_t registro, 
                                   uint8_t *datos, 
                                   size_t longitud);

/**
 * @brief Escribe datos en un registro específico de un dispositivo I2C.
 * 
 * @param[in] manejador_bus Manejador del bus I2C.
 * @param[in] direccion_dev Dirección I2C del dispositivo esclavo.
 * @param[in] registro Comando o dirección del registro a escribir.
 * @param[in] datos Buffer con los datos a enviar.
 * @param[in] longitud Cantidad de bytes a escribir.
 * @return ESP_OK si la escritura fue exitosa.
 */
esp_err_t i2c_driver_escribir_registro(i2c_master_bus_handle_t manejador_bus, 
                                      uint8_t direccion_dev, 
                                      uint8_t registro, 
                                      const uint8_t *datos, 
                                      size_t longitud);

#endif // I2C_DRIVER_H