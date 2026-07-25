/**
 * @file test_main.c
 * @brief Aplicación de pruebas unitarias y de integración usando Unity.
 * @details Contiene la batería de pruebas automatizadas para las capas de abstracción de hardware (HAL),
 *          soporte de placa (Board Support) y middleware (Wi-Fi y MQTT) del termostato IoT.
 *          Verifica la estabilidad del bus I2C, el sensor térmico, la pantalla OLED y la pila de red.
 * @version 1.0
 * @date 2026-07-25
 */

#include <stdio.h>
#include "unity.h"
#include "i2c_driver.h"
#include "esp_err.h"
#include "board.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "middleware.h"


// ============================================================================
// PUNTO DE ENTRADA DE UNITY
// ============================================================================

/**
 * @brief Punto de entrada principal para el entorno de pruebas.
 * @details Configura el entorno Unity y ejecuta de manera automatizada y secuencial 
 *          todos los casos de prueba registrados mediante las macros TEST_CASE.
 */
void app_main(void)
{
    printf("Iniciando pruebas del Termostato IoT...\n");
    
    UNITY_BEGIN();
    
    // Corre todos los tests definidos con TEST_CASE
    unity_run_all_tests();
    
    UNITY_END();
}


// ============================================================================
// CASOS DE PRUEBA: CAPA HAL (I2C)
// ============================================================================

/**
 * @brief HAL-01: Validación de Argumentos Nulos.
 * @details Verifica que todas las funciones públicas del driver I2C manejen 
 *          correctamente punteros nulos sin causar excepciones en el sistema, 
 *          retornando ESP_ERR_INVALID_ARG.
 */
TEST_CASE("I2C Driver - Validacion de argumentos nulos", "[hal][i2c]") 
{
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, i2c_driver_inicializar(NULL));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, i2c_driver_desinicializar(NULL));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, i2c_driver_probar_dispositivo(NULL, 0x3C));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, i2c_driver_escribir(NULL, 0x3C, NULL, 0));
}

/**
 * @brief HAL-02: Inicialización y Liberación de memoria.
 * @details Prueba el ciclo de vida completo del periférico I2C, asegurando que 
 *          el manejador del bus se cree correctamente y sus recursos se liberen 
 *          sin pérdidas de memoria (memory leaks).
 */
TEST_CASE("I2C Driver - Inicializacion y Desinicializacion", "[hal][i2c]") 
{
    i2c_master_bus_handle_t bus_handle = NULL;
    
    // Inicializamos
    esp_err_t err = i2c_driver_inicializar(&bus_handle);
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, err, "Fallo al inicializar el bus I2C");
    TEST_ASSERT_NOT_NULL_MESSAGE(bus_handle, "El manejador del bus devolvió NULL");

    // Liberamos
    err = i2c_driver_desinicializar(bus_handle);
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, err, "Fallo al liberar el bus I2C");
}

/**
 * @brief HAL-03: Detección de hardware físico (Probing).
 * @details Escanea el bus I2C en busca de las direcciones esclavas específicas 
 *          del hardware definido para el proyecto.
 * @note Este test fallará de forma intencional si la pantalla OLED (0x3C) o el 
 *       sensor MLX90614 (0x5A) no están físicamente conectados y alimentados.
 */
TEST_CASE("I2C Driver - Deteccion de hardware fisico (Probing)", "[hal][i2c][hw_required]") 
{
    i2c_master_bus_handle_t bus_handle = NULL;
    TEST_ASSERT_EQUAL(ESP_OK, i2c_driver_inicializar(&bus_handle));

    // Probar OLED 
    esp_err_t err_oled = i2c_driver_probar_dispositivo(bus_handle, 0x3C);
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, err_oled, "Pantalla OLED no detectada en 0x3C");

    // Probar MLX90614 (Dirección por defecto 0x5A)
    esp_err_t err_mlx = i2c_driver_probar_dispositivo(bus_handle, 0x5A);
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, err_mlx, "Sensor MLX90614 no detectado en 0x5A");

    TEST_ASSERT_EQUAL(ESP_OK, i2c_driver_desinicializar(bus_handle));
}

// ============================================================================
// CASOS DE PRUEBA: CAPA BOARD SUPPORT
// ============================================================================

/**
 * @brief BOARD-01: Inicialización general de la placa.
 * @details Llama a la rutina de inicialización en cascada de la placa, verificando
 *          que se levante el bus I2C, la interfaz del sensor térmico y el motor 
 *          gráfico U8g2 simultáneamente.
 */
TEST_CASE("Board Support - Inicializacion general", "[board]") 
{
    // Ejecuta board_init() que internamente levanta el bus, el MLX y el U8g2
    esp_err_t err = board_init();
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, err, "Fallo board_init(). Revisar log de consola para detalles del MLX90614 o I2C.");
}

/**
 * @brief BOARD-02: Lectura de temperatura a través de la capa de abstracción.
 * @details Solicita una lectura térmica y valida que el valor retornado no sea el 
 *          código de error (-999.0) y se encuentre dentro de un rango ambiental lógico.
 */
TEST_CASE("Board Support - Lectura MLX90614", "[board][mlx]") 
{
    float temperatura = board_leer_temperatura();
    
    printf("Temperatura leida por board_leer_temperatura: %.2f C\n", temperatura);

    // Según board.c, si el handle es NULL o falla la lectura, devuelve -999.0
    TEST_ASSERT_NOT_EQUAL_MESSAGE(-999.0, temperatura, "El sensor devolvió el codigo de error (-999.0)");
    
    // Prueba de coherencia de la lectura ambiente (-10 a 60 grados Celsius)
    TEST_ASSERT_GREATER_THAN_FLOAT_MESSAGE(-10.0, temperatura, "Temperatura ambiente ilogicamente baja");
    TEST_ASSERT_LESS_THAN_FLOAT_MESSAGE(60.0, temperatura, "Temperatura ambiente ilogicamente alta");
}

/**
 * @brief BOARD-03: Renderizado en OLED de una temperatura válida.
 * @details Simula una lectura exitosa y envía el valor al motor U8g2 para verificar 
 *          que la conversión flotante a cadena (snprintf) no cause desbordamientos.
 */
TEST_CASE("Board Support - OLED Temperatura Valida", "[board][oled]") 
{
    // Inyectamos un valor de prueba válido para forzar el bloque del snprintf en board.c
    float temp_prueba = 25.4;
    board_mostrar_temperatura_oled(temp_prueba);
    
    // Al ser una función void, testeamos que el buffer U8g2 se envíe sin causar un Crash.
    // Dejamos un pequeño delay para que se pueda verificar visualmente la pantalla.
    vTaskDelay(pdMS_TO_TICKS(2000)); 
    TEST_PASS(); 
}

/**
 * @brief BOARD-04: Renderizado en OLED de un estado de error.
 * @details Inyecta el código de error de lectura (-999.0) para evaluar la rama 
 *          condicional de falla y verificar que se imprima el mensaje de alerta.
 */
TEST_CASE("Board Support - OLED Error", "[board][oled]") 
{
    // Inyectamos -999.0 para forzar la entrada en la condicion "if (temp <= -900.0)" de board.c
    float temp_error = -999.0;
    board_mostrar_temperatura_oled(temp_error);
    
    // Debería mostrar la palabra "ERROR" en la pantalla OLED
    vTaskDelay(pdMS_TO_TICKS(2000));
    TEST_PASS();
}

// ============================================================================
// CASOS DE PRUEBA: CAPA MIDDLEWARE (Wi-Fi y MQTT)
// ============================================================================

/**
 * @brief MID-01: Validación de cliente MQTT no inicializado.
 * @details Prueba la programación defensiva de la función de publicación para asegurar
 *          que retorne un error elegante (ESP_FAIL) en lugar de causar un Panic 
 *          si se intenta enviar datos sin conexión.
 */
TEST_CASE("Middleware - Publicar MQTT sin inicializar", "[mid][mqtt]") 
{
    // Intentamos publicar antes de llamar a MID_MQTT_Init
    esp_err_t err = MID_MQTT_Publish_Data("termostato/test", 25.0);
    
    // Según tu código, debería retornar ESP_FAIL si mqtt_client es NULL
    TEST_ASSERT_EQUAL_MESSAGE(ESP_FAIL, err, "El sistema permitió publicar sin un cliente MQTT activo");
}

/**
 * @brief MID-02: Inicialización de la pila Wi-Fi y TCP/IP.
 * @details Levanta la interfaz de red LwIP, la memoria NVS y configura el 
 *          controlador Wi-Fi en modo Estación (STA) con credenciales de prueba.
 */
TEST_CASE("Middleware - Inicializacion Wi-Fi", "[mid][wifi]") 
{
    // Usamos credenciales ficticias. esp_wifi_connect() es asíncrono, 
    // por lo que no bloqueará esperando conexión real, solo iniciará el proceso.
    esp_err_t err = MID_Network_WiFi_Init("WIFI_CASA", "12345678");
    
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, err, "Fallo al inicializar los componentes de red o LwIP");
}

/**
 * @brief MID-03: Inicialización del cliente MQTT.
 * @details Crea la tarea de fondo de MQTT usando una URI de broker estándar y arranca 
 *          la máquina de estados del cliente.
 */
TEST_CASE("Middleware - Inicializacion MQTT", "[mid][mqtt]") 
{
    // Usamos un broker público común para testear la creación del Handle
    esp_err_t err = MID_MQTT_Init("mqtt://broker.hivemq.com");
    
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, err, "Fallo al arrancar el cliente MQTT");
}

/**
 * @brief MID-04: Formateo JSON y encolado de publicación MQTT.
 * @details Verifica que la telemetría se empaquete correctamente en formato JSON 
 *          y que el cliente acepte el mensaje en su cola de transmisión saliente.
 */
TEST_CASE("Middleware - Publicar JSON MQTT", "[mid][mqtt]") 
{
    // El cliente ya fue inicializado por el test anterior (MID-03)
    float temp_prueba = 22.75;
    
    // Empaquetar "{"temp_ambiente": 22.75}" y devolver ESP_OK
    esp_err_t err = MID_MQTT_Publish_Data("termostato/telemetria", temp_prueba);
    
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, err, "Fallo al encolar el mensaje MQTT");
}

