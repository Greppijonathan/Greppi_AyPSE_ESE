# AyPSE - Documentación del proyecto "Termostato IoT"

Este proyecto consiste en la implementación de un termostato basado en un sensor de temperatura MLX90614. Para la presentación de los datos se utilizó un display OLED SSD1306 y el envío de datos de forma remota a un broker público en HiveMQ.

---

## 📐 Enfoque de la arquitectura en capas

A continuación se describe el modelo de capas implementado, desde el nivel más alto de abstracción hasta el más bajo. El diseño garantiza un bajo acoplamiento aislando el hardware específico a través de una HAL.

| Capa | Descripción | Dependencias (Consume a) | Archivos del proyecto |
| :--- | :--- | :--- | :--- |
| **apps** | Lógica de negocio y propósito principal del producto. Coordina el flujo general. | `board_support`, `middleware` | `main.c` |
| **board_support** | Inicializa los periféricos del proyecto, gestiona la lectura de temperatura y la actualización de la pantalla OLED. | `hardware`, `drivers_hal` | `board.h`, `board.c` |
| **middleware** | Software que actúa como puente y provee servicios estándar. En esta capa se incluye lo referido a red y MQTT. | APIs de red (ESP-IDF) | `middleware.h`, `middleware.c` |
| **hardware** | Implementación de los controladores (drivers instalados) específicos para el sensor MLX90614 y la pantalla OLED U8g2.Se modificaron las funciones para que realicen operaciones de W/R usando la capa drivers_hal. | `drivers_hal` | `mlx90614.h`, `mlx90614.c`, `u8g2.h`, `u8g2.c` |
| **drivers_hal** | Capa de Abstracción de Hardware (HAL). Contiene la implementación genérica del protocolo I2C en español. | **Ninguna (Capa base)** | `i2c_driver.h`, `i2c_driver.c` |

---

## 📂 Índice de Documentación Técnica

Cada nivel de esta arquitectura cuenta con su propia documentación detallada explicando su implementación específica en el código fuente:

1. [Capa de Hardware](./01_hardware.md)
2. [Capa de Drivers y HAL](./02_drivers_hal.md)
3. [Capa de Soporte de Placa (Board Support)](./03_board_support.md)
4. [Capa de Middleware](./04_middleware.md)
5. [Capa de Aplicación](./05_apps.md)