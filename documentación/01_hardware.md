# Capa de Hardware

Esta capa representa el nivel más bajo de la arquitectura de software. Contiene las librerías y drivers de terceros encargados de interactuar directamente con los componentes físicos del proyecto: el sensor de temperatura **MLX90614** y la pantalla **OLED SSD1306**.

Dado que estas librerías encapsulan una gran cantidad de funcionalidades, esta documentación detalla exclusivamente las funciones que el proyecto consume a través de la capa superior (`board_support`).

---

## 🌡️ Sensor Infrarrojo MLX90614

Se accede al sensor por medio de protocolo i2c. A continuacion se detallan las funciones consumidas de la libreria. 
### Funciones Implementadas

| Función | Propósito en el Proyecto |
| :--- | :--- |
| `mlx90614_init()` | Inicializa el driver del sensor vinculándolo al bus I2C maestro y configurando su dirección física por defecto. |
| `mlx90614_get_ta()` | Lee la Temperatura Ambiente (TA) registrada por el sensor y devuelve el valor en grados Celsius. |

---

## 📺 Pantalla OLED SSD1306 (Librería U8g2)

Para el manejo del display monocromático de 128x64 píxeles se optó por **U8g2**, una librería gráfica. El firmware implementa funciones de retardo y comunicación I2C mediante *callbacks* (`u8g2_esp32_i2c_byte_cb` y `u8g2_esp32_gpio_and_delay_cb`) para compatibilizar la librería con las APIs de ESP-IDF v6.

### Funciones de Configuración y Control

| Función | Propósito en el Proyecto |
| :--- | :--- |
| `u8g2_Setup_ssd1306_i2c_128x64_noname_f()` | Configura la instancia de la pantalla, especificando el controlador (SSD1306), resolución, tipo de buffer completo y los callbacks de hardware. |
| `u8g2_InitDisplay()` | Envía la secuencia de comandos de inicialización requerida por el hardware del SSD1306. |
| `u8g2_SetPowerSave()` | Controla el modo de energía. Se utiliza para encender (despertar) la pantalla enviando un `0` como argumento. |

### Funciones de Dibujo 

U8g2 opera mediante un buffer en memoria RAM que se modifica y luego se envía completo a la pantalla.

| Función | Propósito en el Proyecto |
| :--- | :--- |
| `u8g2_ClearBuffer()` | Limpia el buffer interno de dibujo (pantalla en negro) antes de componer un nuevo fotograma. |
| `u8g2_SetFont()` | Define la tipografía a utilizar. El proyecto alterna entre `ncenB08_tr` para el encabezado y `profont29_tf` para el valor numérico. |
| `u8g2_DrawStr()` | Dibuja una cadena de texto en coordenadas cartesianas (X, Y) específicas de la pantalla. |
| `u8g2_DrawHLine()` | Dibuja la línea horizontal divisoria entre el título "TEMPERATURA" y el valor numérico. |
| `u8g2_SendBuffer()` | Transfiere el contenido del buffer interno a la memoria RAM de la pantalla para actualizar la imagen visible. |