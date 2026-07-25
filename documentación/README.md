# AyPSE - Documentación del proyecto "Termostato IoT"

Este proyecto consiste en la implementación de un termostato basado en un sensor de temperatura MLX90614. Para la presentación de los datos se utilizó un display OLED SSD1306 y el envío de datos de forma remota a un broker público en HiveMQ.

---

## 📐 Enfoque de la arquitectura en capas

A continuación se describe el modelo de capas implementado, desde el nivel más alto de abstracción hasta el más bajo:

| Capa | Descripción | Archivos del proyecto |
| :--- | :--- | :--- |
| **apps** | Lógica de negocio y propósito principal del producto. No interactúa directamente con el hardware, usa las capas inferiores. | `main.c` |
| **board_support** | En esta capa se incluye lo necesario para inicializar los periféricos del proyecto, acceder a la lectura de temperatura y actualización de la pantalla OLED. | `board.h`, `board.c` |
| **middleware** | Software que actúa como puente y provee servicios estándar. En esta capa se incluye lo referido a red y MQTT. | `middleware.h`, `middleware.c` |
| **drivers_hal** | Capa de Abstracción de Hardware. Aquí se incluye el manejo del protocolo I2C. | `i2c_master.h`, `i2c_master.c` |
| **hardware** | En esta capa se implementaron las librerías para el manejo del sensor de temperatura y pantalla OLED. | `mlx90614.h`, `mlx90614.c`, `u8g2.h`, `u8g2.c` |

---

## 📂 Índice de Documentación Técnica

Cada nivel de esta arquitectura cuenta con su propia documentación detallada explicando su implementación específica en el código fuente:

1. [Capa de Hardware](./01_hardware.md)
2. [Capa de Drivers y HAL](./02_drivers_hal.md)
3. [Capa de Soporte de Placa (Board Support)](./03_board_support.md)
4. [Capa de Middleware](./04_middleware.md)
5. [Capa de Aplicación](./05_apps.md)