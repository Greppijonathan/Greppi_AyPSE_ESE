#include "i2c_driver.h"
#include "esp_log.h"

static const char *TAG = "I2C_HAL";

esp_err_t i2c_master_init(i2c_master_bus_handle_t *bus_handle) {
    i2c_master_bus_config_t i2c_bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_MASTER_NUM,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    esp_err_t err = i2c_new_master_bus(&i2c_bus_config, bus_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error inicializando bus I2C");
        return err;
    }

    ESP_LOGI(TAG, "Bus I2C inicializado correctamente");
    return ESP_OK;
}