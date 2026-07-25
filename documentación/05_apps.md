# Capa de Aplicación

Esta es la capa superior de la arquitectura. Contiene la lógica de negocio del termostato IoT y es la encargada de coordinar las acciones de las capas inferiores sin interactuar directamente con el hardware ni con los registros del microcontrolador. 

El flujo de ejecución está gobernado por **FreeRTOS**, implementando un diseño concurrente basado en tareas independientes que se comunican de forma segura mediante colas (*Queues*).

---

## 🚀 Punto de Entrada (`app_main`)

La función principal del firmware tiene una única responsabilidad: inicializar los servicios en el orden correcto y delegar el control al RTOS.

**Secuencia de inicialización:**
1. **Hardware:** Llama a `board_init()` para levantar buses y periféricos. Si falla, el sistema se detiene.
2. **Red Wi-Fi:** Llama al middleware para conectar el equipo a la red local usando las credenciales predefinidas.
3. **Espera de Red:** Ejecuta un retardo bloqueante de 5 segundos para dar tiempo a que se asigne una IP (DHCP) antes de intentar conectar a la nube.
4. **MQTT:** Inicializa el cliente MQTT hacia el broker público de HiveMQ.
5. **IPC (Inter-Process Communication):** Crea una cola de FreeRTOS (`temp_queue`) con capacidad para 1 solo elemento de tipo `float`.
6. **Lanzamiento de Tareas:** Crea y planifica las dos tareas principales del sistema.

---

## 🔄 Arquitectura Concurrente (FreeRTOS)

El firmware utiliza un patrón **Productor - Consumidor** para separar las operaciones de hardware (rápidas) de las operaciones de red (lentas o con latencia variable).

### 1. Tarea Productora: `task_medir_y_oled`
* **Prioridad:** 5 (Alta).
* **Frecuencia:** Cada 2 segundos.
* **Propósito:** Leer el sensor, mostrar el valor en la pantalla OLED de forma instantánea y enviar el dato a la cola de comunicación.
* **Manejo de Cola:** Utiliza `xQueueOverwrite()`. Al ser una cola de un solo espacio, esta función garantiza que el dato más viejo se sobrescriba por el más nuevo si la tarea de red aún no lo procesó. Esto asegura que la pantalla local nunca se congele por culpa de una mala conexión a internet.

### 2. Tarea Consumidora: `task_mqtt_publish`
* **Prioridad:** 4 (Media).
* **Frecuencia:** Espera de forma indefinida hasta recibir un dato, y luego aplica una cadencia máxima de 5 segundos.
* **Propósito:** Extraer la temperatura de la cola y publicarla en el tópico MQTT (`greppi/aypse/temp`) mediante la capa de Middleware.
* **Manejo de Cola:** Utiliza `xQueueReceive()` con un retardo máximo (`portMAX_DELAY`). La tarea permanece "dormida" sin consumir CPU hasta que la tarea productora inserta una nueva lectura en la cola.

---

## 🔑 Credenciales y Configuración

Los parámetros críticos de la aplicación se definen mediante macros en la cabecera del archivo, lo que facilita su modificación sin alterar la lógica de las tareas:

| Macro | Valor | Descripción |
| :--- | :--- | :--- |
| `WIFI_SSID` | `"mi red"` | Nombre de la red Wi-Fi local. |
| `WIFI_PASS` | `"mi clave"` | Contraseña de la red Wi-Fi. |
| `MQTT_BROKER_URL` | `"mqtt://broker.hivemq.com"` | URL del servidor MQTT público. |