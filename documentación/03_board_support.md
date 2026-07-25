# Capa de Soporte de Placa (Board Support)

Esta capa actúa como el intermediario principal entre la aplicación de alto nivel y el hardware físico. Su objetivo es orquestar la inicialización de los buses de comunicación, registrar los dispositivos periféricos y proveer una API simplificada para que la capa de aplicación pueda interactuar con el entorno sin conocer los detalles técnicos del microcontrolador.

---

## 🚀 Inicialización Global

La configuración inicial unifica el arranque de todos los subsistemas físicos requeridos por el termostato.

### `board_init()`

**Firma:** `esp_err_t board_init(void)`

Esta función se ejecuta al inicio del programa y se encarga de levantar secuencialmente el hardware:
1. **Bus Maestro:** Llama a la capa HAL para inicializar el bus I2C interno.
2. **Sensor Térmico:** Configura y registra el sensor MLX90614 en el bus I2C (dirección por defecto).
3. **Pantalla OLED:** Registra el dispositivo esclavo SSD1306 (dirección `0x3C`) en el mismo bus I2C.
4. **Librería Gráfica:** Vincula los callbacks de comunicación a la librería U8g2, enciende la pantalla y limpia el buffer gráfico.

**Retorno:** Devuelve `ESP_OK` si todos los periféricos se inicializaron correctamente, o un código de error de ESP-IDF en caso de falla.

---

## 🌡️ Adquisición de Datos

### `board_leer_temperatura()`

**Firma:** `float board_leer_temperatura(void)`

Encapsula la lectura de la Temperatura Ambiente (TA) desde el sensor infrarrojo. 

**Manejo de Errores:** Si el *handle* del sensor es nulo o la lectura por I2C falla, la función retorna el valor bandera `-999.0`. Esto permite que la capa de aplicación detecte fácilmente una falla física sin lidiar con códigos de error complejos.

---

## 📺 Interfaz de Usuario (Display)

### `board_mostrar_temperatura_oled()`

**Firma:** `void board_mostrar_temperatura_oled(float temp)`

Se encarga de formatear y renderizar la información en la pantalla OLED de 128x64. 

**Características de la implementación:**
* Recibe como parámetro directo la temperatura en formato de punto flotante.
* Diseña la interfaz gráfica con un encabezado estático ("TEMPERATURA") y una línea divisoria.
* Si el valor recibido es menor o igual a `-900.0` (indicando el error mencionado en la función de lectura), imprime la palabra **"ERROR"** en pantalla.
* En condiciones normales, formatea el número a un solo decimal (ejemplo: `24.5 C`) utilizando la fuente tipográfica de gran tamaño `profont29`.
* Realiza el envío automático del buffer de memoria para actualizar la imagen visible al instante.

---

## ⚙️ Integración Interna (Callbacks)

Para garantizar la compatibilidad entre la librería externa U8g2 y el driver I2C nativo de ESP-IDF v6, este módulo implementa dos funciones internas estáticas de tipo *callback*:

| Función Interna | Descripción |
| :--- | :--- |
| `u8g2_esp32_i2c_byte_cb` | Intercepta los bytes generados por U8g2 y los empaqueta para transmitirlos mediante `i2c_master_transmit`. |
| `u8g2_esp32_gpio_and_delay_cb` | Vincula las solicitudes de espera de U8g2 con la función de retardo no bloqueante del RTOS (`vTaskDelay`). |