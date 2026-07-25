idf_component_register(SRCS "middleware.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_wifi esp_netif mqtt nvs_flash)

