# Plan de Pruebas: Capa Hardware Abstraction Layer (HAL) - Protocolo I2C

**Módulo:** `drivers_hal`
**Componente:** Bus I2C (Master)
**Microcontrolador:** ESP32-C6

## Resumen
Este conjunto de pruebas verifica la correcta configuración, operación y manejo de errores de las funciones que envuelven el controlador I2C nativo dentro de la capa HAL del proyecto.

## Casos de Prueba

| ID | Nombre de la Prueba | Descripción | Precondiciones | Pasos de Ejecución | Resultado Esperado |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **TC-I2C-01** | Inicialización del Bus | Verifica que el bus I2C se inicializa con los pines y frecuencia correctos. | Microcontrolador encendido. Bus I2C libre. | 1. Llamar a la función de inicialización HAL I2C. | La función retorna `ESP_OK`. El puerto I2C queda configurado. |
| **TC-I2C-02** | Detección de Dispositivo Real (ACK) | Verifica que el bus puede comunicarse con un esclavo real (ej. MLX90614 o U8g2). | TC-I2C-01 aprobado. Sensor conectado en pines I2C. | 1. Enviar comando I2C (ej. *ping* o lectura) a la dirección del esclavo (ej. `0x5A`). | Retorna `ESP_OK` (Se recibe ACK del esclavo). |
| **TC-I2C-03** | Detección de Dispositivo Falso (NACK) | Verifica el correcto manejo de errores al direccionar un esclavo que no existe. | TC-I2C-01 aprobado. | 1. Enviar comando I2C a una dirección vacía (ej. `0x7F`). | Retorna un código de error (ej. `ESP_ERR_TIMEOUT` o `ESP_FAIL`). |
| **TC-I2C-04** | Desinicialización del Bus | Verifica que los recursos de hardware se liberan correctamente. | TC-I2C-01 aprobado. Bus ocupado. | 1. Llamar a la función HAL para desinstalar el I2C. | Retorna `ESP_OK`. El recurso de memoria y hardware queda libre. |