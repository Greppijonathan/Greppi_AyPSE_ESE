#include <stdio.h>
#include "unity.h"
#include "esp_err.h"

// 1. Incluimos tu capa HAL real (el compilador irá a buscar el archivo i2c_driver.h)
#include "i2c_driver.h"

// 2. Constantes exclusivas de prueba 
// (Los pines y frecuencias ya los toma automáticamente de i2c_driver.h)
#define MLX90614_I2C_ADDR       0x5A    // Dirección clásica del sensor de temperatura
#define FAKE_I2C_ADDR           0x7F    // Una dirección que sabemos que no existe

// ============================================================================
// CASOS DE PRUEBA: I2C HAL
// ============================================================================

TEST_CASE("TC-I2C-01: Inicializacion del bus I2C exitosa", "[HAL_I2C]") {
    // Act: Llamamos a tu función de inicialización del HAL
    // esp_err_t err = hal_i2c_init(); // <-- Descomentá y usá tu función real
    
    // Simulación temporal para que compile hasta que pongas tu función:
    esp_err_t err = ESP_OK; 

    // Assert: Verificamos que no hubo errores en la configuración
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, err, "Fallo al inicializar el driver I2C");
}

TEST_CASE("TC-I2C-02: Dispositivo real responde (ACK)", "[HAL_I2C]") {
    // Arrange: Aseguramos que el bus esté iniciado
    // hal_i2c_init(); 
    
    // Act: Hacemos un "ping" o lectura a la dirección del sensor real
    // esp_err_t err = hal_i2c_check_device(MLX90614_I2C_ADDR); // <-- Tu función
    esp_err_t err = ESP_OK; // Placeholder
    
    // Assert: El dispositivo debe responder (ACK)
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, err, "El sensor MLX90614 no respondio al ACK");
}

TEST_CASE("TC-I2C-03: Dispositivo inexistente devuelve error (NACK)", "[HAL_I2C]") {
    // Arrange
    // hal_i2c_init();
    
    // Act: Intentamos hablar con una dirección que no existe
    // esp_err_t err = hal_i2c_check_device(FAKE_I2C_ADDR); // <-- Tu función
    esp_err_t err = ESP_FAIL; // Placeholder, asumiendo que falla como debe ser
    
    // Assert: Tiene que fallar, por ende NO debe ser ESP_OK
    TEST_ASSERT_NOT_EQUAL_MESSAGE(ESP_OK, err, "El sistema no detecto la ausencia del dispositivo");
}

TEST_CASE("TC-I2C-04: Desinicializacion del bus I2C exitosa", "[HAL_I2C]") {
    // Arrange: Iniciamos primero para luego cerrarlo
    // hal_i2c_init();
    
    // Act: Cerramos el bus y liberamos recursos
    // esp_err_t err = hal_i2c_deinit(); // <-- Tu función
    esp_err_t err = ESP_OK; // Placeholder
    
    // Assert
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, err, "Fallo al desinstalar el driver I2C");
}

// ============================================================================
// APLICACIÓN PRINCIPAL
// ============================================================================
void app_main(void) {
    printf("Iniciando bateria de pruebas de Capa HAL (I2C)...\n");
    
    UNITY_BEGIN();
    unity_run_all_tests();
    UNITY_END();
}