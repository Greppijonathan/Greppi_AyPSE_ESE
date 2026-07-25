cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS 
    "../board_support" 
    "../drivers_hal" 
    "../middleware"
    "../managed_components"
    "../hardware"
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(test_app)