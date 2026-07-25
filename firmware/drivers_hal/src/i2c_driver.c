#include "i2c_driver.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "I2C_DRIVER_HAL";

esp_err_t i2c_driver_inicializar(i2c_master_bus_handle_t *manejador_bus) {
    if (manejador_bus == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_MAESTRO_NUM,
        .scl_io_num = I2C_MAESTRO_SCL_IO,
        .sda_io_num = I2C_MAESTRO_SDA_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    esp_err_t err = i2c_new_master_bus(&i2c_mst_config, manejador_bus);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error al crear el bus I2C maestro: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "Bus I2C inicializado con exito (SDA: %d, SCL: %d)", 
             I2C_MAESTRO_SDA_IO, I2C_MAESTRO_SCL_IO);
    return ESP_OK;
}

esp_err_t i2c_driver_desinicializar(i2c_master_bus_handle_t manejador_bus) {
    if (manejador_bus == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    return i2c_del_master_bus(manejador_bus);
}

esp_err_t i2c_driver_probar_dispositivo(i2c_master_bus_handle_t manejador_bus, uint16_t direccion_dev) {
    if (manejador_bus == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    return i2c_master_probe(manejador_bus, direccion_dev, 100);
}

esp_err_t i2c_driver_escribir(i2c_master_bus_handle_t manejador_bus, 
                             uint8_t direccion_dev, 
                             const uint8_t *datos, 
                             size_t longitud) {
    if (manejador_bus == NULL || datos == NULL || longitud == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = direccion_dev,
        .scl_speed_hz = I2C_MAESTRO_FREQ_HZ,
    };

    i2c_master_dev_handle_t dev_handle = NULL;

    esp_err_t err = i2c_master_bus_add_device(manejador_bus, &dev_cfg, &dev_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error al agregar dispositivo I2C 0x%02X: %s", direccion_dev, esp_err_to_name(err));
        return err;
    }

  
    err = i2c_master_transmit(dev_handle, datos, longitud, -1);


    i2c_master_bus_rm_device(dev_handle);

    return err;
}

esp_err_t i2c_driver_leer_registro(i2c_master_bus_handle_t manejador_bus, 
                                   uint8_t direccion_dev, 
                                   uint8_t registro, 
                                   uint8_t *datos, 
                                   size_t longitud) {
    if (manejador_bus == NULL || datos == NULL || longitud == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = direccion_dev,
        .scl_speed_hz = I2C_MAESTRO_FREQ_HZ,
    };

    i2c_master_dev_handle_t dev_handle = NULL;
    esp_err_t err = i2c_master_bus_add_device(manejador_bus, &dev_cfg, &dev_handle);
    if (err != ESP_OK) {
        return err;
    }

    err = i2c_master_transmit_receive(dev_handle, &registro, 1, datos, longitud, -1);

    i2c_master_bus_rm_device(dev_handle);
    return err;
}

esp_err_t i2c_driver_escribir_registro(i2c_master_bus_handle_t manejador_bus, 
                                      uint8_t direccion_dev, 
                                      uint8_t registro, 
                                      const uint8_t *datos, 
                                      size_t longitud) {
    if (manejador_bus == NULL || (datos == NULL && longitud > 0)) {
        return ESP_ERR_INVALID_ARG;
    }

    
    uint8_t buffer_tx[longitud + 1];
    buffer_tx[0] = registro;
    if (longitud > 0) {
        memcpy(&buffer_tx[1], datos, longitud);
    }

    return i2c_driver_escribir(manejador_bus, direccion_dev, buffer_tx, longitud + 1);
}