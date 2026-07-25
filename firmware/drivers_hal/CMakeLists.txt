idf_component_register(
    SRCS "src/i2c_driver.c"
    INCLUDE_DIRS "inc"
    REQUIRES esp_driver_i2c
)