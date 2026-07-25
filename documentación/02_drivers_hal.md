# Capa de Drivers y HAL (Hardware Abstraction Layer)

Esta capa tiene como objetivo abstraer la complejidad de los periféricos físicos del microcontrolador ESP32-C6. Al encapsular la inicialización del hardware nativo, las capas superiores (como el *Board Support*) pueden comunicarse con los buses de datos sin necesidad de interactuar con los registros internos o configuraciones de bajo nivel del chip.

En este proyecto, la capa HAL se encarga exclusivamente de configurar y levantar el bus **I2C Maestro** utilizando la nueva API de controladores nativos de ESP-IDF v6.

---

## 🔌 Configuración Física del Bus I2C

Para estandarizar la conexión con los dispositivos externos (sensor MLX90614 y pantalla OLED SSD1306), se definieron las siguientes macros de hardware en `i2c_driver.h`:

| Parámetro | Valor | Descripción |
| :--- | :--- | :--- |
| `I2C_MASTER_SDA_IO` | **6** | Pin GPIO asignado para la línea de datos (SDA). |
| `I2C_MASTER_SCL_IO` | **7** | Pin GPIO asignado para la línea de reloj (SCL). |
| `I2C_MASTER_FREQ_HZ`| **100000** | Frecuencia del reloj del bus (100 kHz - Modo Estándar). |
| `I2C_MASTER_NUM` | **I2C_NUM_0** | Puerto de hardware interno del ESP32-C6 asignado a este bus. |

---

## 🛠️ API de la Capa HAL

### `i2c_master_init()`

**Firma:** `esp_err_t i2c_master_init(i2c_master_bus_handle_t *bus_handle)`

Esta es la función principal de la capa HAL. Su propósito es inicializar el controlador I2C interno del ESP32-C6 y prepararlo para gestionar la comunicación. 

**Características de la implementación:**
* **Asignación de Pines:** Vincula físicamente los pines GPIO 6 y 7 a las señales SDA y SCL del bus cero.
* **Pull-ups Internos:** Habilita las resistencias *pull-up* internas del microcontrolador (`enable_internal_pullup = true`), asegurando los estados lógicos altos del bus sin requerir hardware externo extra.
* **Filtro Anti-ruido:** Implementa un filtro de transitorios (`glitch_ignore_cnt = 7`) para mejorar la robustez y estabilidad de la comunicación.
* **Paso por Referencia:** Retorna el *handle* (manejador) del bus ya configurado a través del puntero `bus_handle`, permitiendo que las capas superiores puedan acoplar sus dispositivos a este mismo bus I2C.