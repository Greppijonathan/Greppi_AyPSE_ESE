idf_component_register(
    SRCS "src/board.c"
    INCLUDE_DIRS "inc"
    REQUIRES drivers_hal mlx90614 u8g2
)