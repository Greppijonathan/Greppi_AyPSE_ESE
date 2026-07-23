#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include "driver/i2c_master.h"
#include "esp_err.h"

#define I2C_MASTER_SDA_IO           6      
#define I2C_MASTER_SCL_IO           7      
#define I2C_MASTER_FREQ_HZ          100000 
#define I2C_MASTER_NUM              I2C_NUM_0

/**
 * @brief Inicializa el bus I2C maestro con el nuevo driver
 * @param[out] bus_handle Puntero al handle del bus I2C creado
 */
esp_err_t i2c_master_init(i2c_master_bus_handle_t *bus_handle);

#endif // I2C_DRIVER_H