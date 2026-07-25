# Capa de Drivers y HAL (Hardware Abstraction Layer)

Esta capa tiene como objetivo abstraer la complejidad de los periféricos físicos del microcontrolador ESP32-C6. Al encapsular la inicialización y manipulación del hardware nativo, las capas superiores (como el *Board Support*) pueden comunicarse con los buses de datos sin necesidad de interactuar con los registros internos, configuraciones de bajo nivel o las APIs nativas de Espressif (ESP-IDF).

En este proyecto, la capa HAL se encarga exclusivamente de configurar y gestionar el bus **I2C Maestro**, estandarizando todas las firmas de funciones al español para mantener consistencia en la arquitectura.

---

## 🔌 Configuración Física del Bus I2C

Para estandarizar la conexión con los dispositivos externos (sensor MLX90614 y pantalla OLED SSD1306), se definieron las siguientes macros de hardware en `i2c_driver.h`:

| Parámetro | Valor | Descripción |
| :--- | :--- | :--- |
| `I2C_MAESTRO_SDA_IO` | **6** | Pin GPIO asignado para la línea de datos (SDA). |
| `I2C_MAESTRO_SCL_IO` | **7** | Pin GPIO asignado para la línea de reloj (SCL). |
| `I2C_MAESTRO_FREQ_HZ`| **100000** | Frecuencia del reloj del bus (100 kHz - Modo Estándar). |
| `I2C_MAESTRO_NUM` | **I2C_NUM_0** | Puerto de hardware interno del ESP32-C6 asignado a este bus. |

---

## 🛠️ API de la Capa HAL

A continuación se detallan las funciones expuestas por la HAL para el consumo por parte de la capa de Hardware y Soporte de Placa:

### 1. Inicialización y Control

| Función | Descripción |
| :--- | :--- |
| `i2c_driver_inicializar()` | Inicializa el controlador I2C interno, habilitando *pull-ups* internos y un filtro anti-ruido (`glitch_ignore_cnt = 7`). Retorna el *handle* del bus configurado. |
| `i2c_driver_desinicializar()` | Libera los recursos de memoria y hardware asignados al bus I2C maestro. |
| `i2c_driver_probar_dispositivo()` | Verifica si un dispositivo responde con un ACK en una dirección I2C específica. Útil para detectar la presencia física de la pantalla o el sensor durante el arranque. |

### 2. Operaciones de Transmisión de Datos

Estas funciones abstraen el manejo temporal de los *device handles* de ESP-IDF, aislando a las capas superiores de esta complejidad.

| Función | Descripción |
| :--- | :--- |
| `i2c_driver_escribir()` | Transmite un buffer de bytes directamente al dispositivo (sin registro explícito). Es utilizado de forma intensiva por la librería U8g2 para enviar comandos y datos de visualización. |
| `i2c_driver_leer_registro()` | Escribe una dirección de sub-registro y luego lee los datos devueltos por el esclavo. Esencial para obtener las mediciones precisas del sensor MLX90614. |
| `i2c_driver_escribir_registro()` | Escribe en un sub-registro específico de un dispositivo I2C, concatenando automáticamente el registro y los datos de manera transparente. |