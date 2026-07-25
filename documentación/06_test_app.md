# Plan y Reporte de Pruebas: Firmware Termostato IoT (ESP32-C6)

**Microcontrolador:** ESP32-C6  
**Framework de Pruebas:** Unity (Nativo ESP-IDF)  
**Componente de Test:** `test_app`  

## Resumen
Este documento detalla la batería de pruebas unitarias e integración para validar la estabilidad de la arquitectura por capas del termostato IoT. Las pruebas abarcan desde la interacción con los periféricos de bajo nivel (HAL) y la abstracción del hardware de la placa (Board Support), hasta los servicios de infraestructura de red y telemetría (Middleware).

---

## 1. Capa Hardware Abstraction Layer (HAL) - Protocolo I2C

**Módulo:** `drivers_hal`  
**Etiqueta Unity:** `[hal][i2c]`

| ID | Caso de Prueba | Descripción / Validación | Resultado Esperado |
| :--- | :--- | :--- | :--- |
| **HAL-01** | Validación de Argumentos Nulos | Evalúa la robustez defensiva pasando punteros `NULL` a las funciones `i2c_driver_inicializar`, `desinicializar`, `probar_dispositivo` y `escribir`. | Retorna `ESP_ERR_INVALID_ARG` en cada llamada sin causar *Crash*. |
| **HAL-02** | Inicialización y Desinicialización | Modela el ciclo de vida completo del bus I2C Máster, asegurando la correcta asignación y liberación del *handle*. | Retorna `ESP_OK` y deja el manejador del bus limpio en memoria. |
| **HAL-03** | Detección de Hardware Físico (Probing) | Efectúa un escaneo I2C (*ping*) en las direcciones esclavas asociadas al hardware físico conectado a la placa. | Retorna `ESP_OK` detectando la pantalla OLED (`0x3C`) y el sensor MLX90614 (`0x5A`). |

---

## 2. Capa Board Support Package (BOARD)

**Módulo:** `board`  
**Etiqueta Unity:** `[board]`

| ID | Caso de Prueba | Descripción / Validación | Resultado Esperado |
| :--- | :--- | :--- | :--- |
| **BOARD-01** | Inicialización General de la Placa | Ejecuta `board_init()`, verificando la inicialización secuencial en cascada del bus I2C, la interfaz del sensor térmico y el motor gráfico U8g2. | Retorna `ESP_OK`. Todos los subsistemas quedan listos para operar. |
| **BOARD-02** | Lectura de Sensor MLX90614 | Obtiene el valor térmico ambiental a través de la función de abstracción `board_leer_temperatura()`. | Valor diferente al código de falla (`-999.0`) y dentro de un rango coherente (-10 °C a 60 °C). |
| **BOARD-03** | Renderizado OLED Temperatura Válida | Inyecta una lectura válida (25.4 °C) para validar la conversión de formato flotante a cadena y el envío del búfer gráfico. | Dibuja la temperatura correctamente en pantalla sin causar desbordamientos de búfer. |
| **BOARD-04** | Renderizado OLED Estado de Error | Inyecta el valor de falla (`-999.0`) para forzar la rama condicional de manejo de excepciones en la pantalla. | Muestra la palabra `"ERROR"` de forma clara en el display OLED. |

---

## 3. Capa Middleware (Red y Telemetría)

**Módulo:** `middleware`  
**Etiqueta Unity:** `[mid]`

| ID | Caso de Prueba | Descripción / Validación | Resultado Esperado |
| :--- | :--- | :--- | :--- |
| **MID-01** | Publicación MQTT sin Inicializar | Ejecuta `MID_MQTT_Publish_Data()` antes de crear el cliente MQTT para probar la protección contra punteros nulos. | Retorna `ESP_FAIL` de forma controlada sin provocar un reinicio por *Panic*. |
| **MID-02** | Inicialización de Red Wi-Fi | Inicializa la memoria NVS, la pila TCP/IP LwIP, el bucle de eventos por defecto y el controlador Wi-Fi en modo Estación (STA). | Retorna `ESP_OK` tras configurar la interfaz de red. |
| **MID-03** | Inicialización del Cliente MQTT | Instancia la estructura del cliente MQTT asociando la URL del broker (`mqtt://broker.hivemq.com`) y arranca la tarea en segundo plano. | Retorna `ESP_OK` dejando la máquina de estados del cliente operativa. |
| **MID-04** | Formateo JSON y Encolado MQTT | Formatea un flotante a una cadena JSON (`{"temp_ambiente": 22.75}`) y solicita su publicación con QoS 1. | Retorna `ESP_OK` indicando que el mensaje fue aceptado en la cola de salida. |

---

## Ejecución de Pruebas

Para compilar y ejecutar la batería completa de pruebas desde la terminal en el directorio `test_app`:

```bash
idf.py build flash monitor