# Capa de Middleware

La capa de Middleware provee servicios de infraestructura y comunicación estándar para la aplicación. En este proyecto, actúa como un puente para gestionar la conectividad a Internet a través de Wi-Fi y el envío de telemetría hacia la nube utilizando el protocolo MQTT.

Al encapsular estas tecnologías, la lógica de negocio (capa de aplicación) no necesita conocer los detalles de la pila TCP/IP, el manejo de memoria flash o la gestión de sockets.

---

## 🌐 Conectividad Wi-Fi

El microcontrolador ESP32-C6 requiere la inicialización de varios subsistemas (como la memoria no volátil y la interfaz de red) antes de poder establecer un enlace inalámbrico.

### `MID_Network_WiFi_Init()`

**Firma:** `esp_err_t MID_Network_WiFi_Init(const char* ssid, const char* password)`

Esta función orquesta el levantamiento completo de la pila de red y conecta el dispositivo a un punto de acceso (Router).

**Flujo de ejecución:**
1. **Inicialización de NVS (Non-Volatile Storage):** Levanta el sistema de archivos flash, el cual es un requisito obligatorio de los drivers de Wi-Fi de Espressif para guardar calibraciones de radiofrecuencia. Si la partición está corrupta o llena, la borra y la reinicializa.
2. **Interfaz de Red:** Crea el bucle de eventos por defecto (`esp_event_loop_create_default`) y levanta la interfaz de red LwIP (`esp_netif_init`).
3. **Modo Estación (STA):** Configura la radio Wi-Fi en modo cliente (Station), le inyecta las credenciales (SSID y Contraseña) pasadas por parámetro y ordena la conexión al punto de acceso.

---

## 📡 Protocolo MQTT

El envío de telemetría hacia el broker público (HiveMQ) se gestiona mediante el cliente MQTT nativo de ESP-IDF.

### `MID_MQTT_Init()`

**Firma:** `esp_err_t MID_MQTT_Init(const char* broker_url)`

Inicializa el servicio de mensajería y establece la conexión persistente con el servidor en la nube.

**Características:**
* Recibe como parámetro la URL o IP del broker MQTT.
* Construye el *handle* (`esp_mqtt_client_handle_t`) de forma global para mantener la sesión abierta.
* Arranca la tarea en segundo plano del cliente MQTT para gestionar *ping-req*, *keep-alive* y reconexiones automáticas.

### `MID_MQTT_Publish_Data()`

**Firma:** `esp_err_t MID_MQTT_Publish_Data(const char* topic, float temp_ambiente)`

Toma el dato en crudo proveniente de la capa de Hardware, lo empaqueta y lo despacha a la nube.

**Características de la implementación:**
* **Formateo JSON:** Envuelve el valor flotante de la temperatura dentro de un string con estructura JSON (`{"temp_ambiente": 24.50}`). Esto estandariza el payload para que cualquier plataforma de IoT o Dashboard lo pueda parsear fácilmente.
* **Calidad de Servicio (QoS):** Utiliza QoS 1, lo que garantiza que el mensaje se entregue al menos una vez al broker, aumentando la fiabilidad de los reportes.
* **Manejo de Seguridad:** Verifica que el cliente MQTT esté inicializado antes de intentar publicar, evitando desbordamientos de memoria o fallos fatales en el procesador.