cmake_minimum_required(VERSION 3.16)

# Estructura de la arquitectura d capas 
set(EXTRA_COMPONENT_DIRS 
    "apps" 
    "board_support" 
    "drivers_hal" 
    "hardware" 
    "middleware" 
    "test_app"
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(AyPSE_ESE)