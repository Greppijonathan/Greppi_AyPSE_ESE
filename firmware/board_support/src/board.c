#include "board.h"
#include "i2c_driver.h"
#include "mlx90614.h"
#include "u8g2.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

static const char *TAG = "BOARD";

// Handles globales
static i2c_master_bus_handle_t bus_handle = NULL;
static mlx90614_handle_t mlx_handle = NULL;
static u8g2_t u8g2; 

// Dirección I2C por defecto del display 
#define OLED_I2C_ADDR 0x3C 

// Handle del dispositivo OLED en el bus I2C
static i2c_master_dev_handle_t oled_dev_handle = NULL;

/**
 * @brief Callback de transmisión de bytes para u8g2 
 */
static uint8_t u8g2_esp32_i2c_byte_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    static uint8_t buffer[32];
    static uint8_t buf_idx = 0;

    switch (msg) {
        case U8X8_MSG_BYTE_SEND: {
            uint8_t *data = (uint8_t *)arg_ptr;
            while (arg_int > 0) {
                buffer[buf_idx++] = *data;
                data++;
                arg_int--;
            }
            break;
        }
        case U8X8_MSG_BYTE_INIT:
            break;
        case U8X8_MSG_BYTE_SET_DC:
            break;
        case U8X8_MSG_BYTE_START_TRANSFER:
            buf_idx = 0;
            break;
        case U8X8_MSG_BYTE_END_TRANSFER:
            if (oled_dev_handle != NULL && buf_idx > 0) {
                i2c_master_transmit(oled_dev_handle, buffer, buf_idx, -1);
            }
            break;
        default:
            return 0;
    }
    return 1;
}

/**
 * @brief Callback de retardos para u8g2
 */
static uint8_t u8g2_esp32_gpio_and_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch (msg) {
        case U8X8_MSG_DELAY_MILLI:
            vTaskDelay(pdMS_TO_TICKS(arg_int));
            break;
        default:
            return 0;
    }
    return 1;
}

esp_err_t board_init(void) {
    ESP_LOGI(TAG, "Inicializando hardware de la placa!");
    
    // Bus I2C Maestro 
    esp_err_t err = i2c_master_init(&bus_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error al arrancar el bus I2C maestro!");
        return err;
    }

    // Sensor MLX90614
    mlx90614_config_t mlx_cfg = {
        .mlx90614_device = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address = MLX90614_DEFAULT_ADDRESS,
            .scl_speed_hz = I2C_MASTER_FREQ_HZ,
        }
    };
    err = mlx90614_init(bus_handle, &mlx_cfg, &mlx_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error al inicializar el sensor MLX90614!");
        return err;
    }

    //Registrar pantalla OLED en el bus I2C
    i2c_device_config_t oled_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = OLED_I2C_ADDR,
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,
    };
    err = i2c_master_bus_add_device(bus_handle, &oled_cfg, &oled_dev_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error registrando pantalla OLED en bus I2C");
        return err;
    }

    //Inicializar u8g2 (Pantalla)
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(
        &u8g2,
        U8G2_R0,
        u8g2_esp32_i2c_byte_cb,
        u8g2_esp32_gpio_and_delay_cb
    );
    
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0); // Enciende la pantalla
    u8g2_ClearBuffer(&u8g2);

    ESP_LOGI(TAG, "Hardware inicializado y listo!");
    return ESP_OK;
}

float board_leer_temperatura(void) {
    float temperatura_obj = 0.0;
    if (mlx_handle == NULL) return -999.0;

    esp_err_t err = mlx90614_get_to(mlx_handle, &temperatura_obj);
    if (err != ESP_OK) return -999.0;
    
    return temperatura_obj;
}

void board_mostrar_temperatura_oled(float temp) {

    char str_buffer[32];
    
    u8g2_ClearBuffer(&u8g2);
    
    // Encabezado
    u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr);
    u8g2_DrawStr(&u8g2, 20, 15, "TEMPERATURA");
    u8g2_DrawHLine(&u8g2, 0, 20, 128);

    // Lectura de Temperatura
    u8g2_SetFont(&u8g2, u8g2_font_profont29_tf);
    if (temp <= -900.0) {
        u8g2_DrawStr(&u8g2, 15, 50, "ERROR");
    } else {
        snprintf(str_buffer, sizeof(str_buffer), "%.1f C", temp);
        u8g2_DrawStr(&u8g2, 15, 52, str_buffer);
    }

    u8g2_SendBuffer(&u8g2);
}