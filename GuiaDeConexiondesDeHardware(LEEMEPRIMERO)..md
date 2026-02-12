# 🗺️ Guía de Conexiones - AricaTux PDA (Open Source)

Este documento detalla el cableado necesario para construir la PDA basada en **ESP32 WROOM-32S** y el módulo de cámara **ESP32-CAM**.

## 📱 1. Pantalla TFT + Touch (ILI9341 2.4"/2.8")

La pantalla y el panel táctil comparten el bus **SPI**. Esto significa que varios cables van a los mismos pines del ESP32.

| Pin Pantalla | Pin ESP32 WROOM | Función | Notas |
| :--- | :--- | :--- | :--- |
| **VCC** | 3.3V o 5V | Alimentación | Revisa si tu pantalla tiene regulador. |
| **GND** | GND | Tierra | Común para todo el sistema. |
| **CS** | GPIO 15 | Chip Select (LCD) | Controla la imagen. |
| **RESET** | GPIO 4 | Reset | Reinicio de pantalla. |
| **DC/RS** | GPIO 2 | Data/Command | Selección de registro. |
| **SDI (MOSI)** | GPIO 23 | SPI Data In | **Compartido** con Touch. |
| **SCK** | GPIO 18 | SPI Clock | **Compartido** con Touch. |
| **LED** | 3.3V | Backlight | Iluminación de fondo. |
| **SDO (MISO)** | GPIO 19 | SPI Data Out | **Compartido** con Touch. |
| **T_CLK** | GPIO 18 | Touch Clock | **Compartido** con LCD. |
| **T_CS** | GPIO 5 | Chip Select (Touch)| Controla el táctil. |
| **T_DIN** | GPIO 23 | Touch Data In | **Compartido** con LCD. |
| **T_DO** | GPIO 19 | Touch Data Out | **Compartido** con LCD. |
| **T_IRQ** | GPIO 27 | Touch Interrupt | Avisa cuando presionas. |

---

## 🔊 2. Audio y Periféricos

| Componente | Pin ESP32 WROOM | Función |
| :--- | :--- | :--- |
| **Buzzer (+)** | GPIO 21 | Beeps de interfaz y cámara. |
| **Buzzer (-)** | GND | Tierra. |

---

## 📸 3. Módulo de Cámara (ESP32-CAM)

Este módulo no requiere cables hacia la PDA (se comunican por WiFi), pero necesita una fuente de poder estable.

| Pin ESP32-CAM | Conexión | Notas |
| :--- | :--- | :--- |
| **5V** | Fuente 5V / Batería | Recomendado usar fuente externa de 2A. |
| **GND** | GND | Tierra común. |
| **GPIO 4** | (Interno) | Flash LED (Controlado por código). |

---

## ⚠️ Notas Críticas para el Montaje

1. **Bus SPI Compartido**: Asegúrate de que los pines 18, 19 y 23 lleguen tanto a los pines de la pantalla como a los del touch. Si el touch no responde, revisa el pin **T_CS (GPIO 5)**.
2. **Alimentación**: El ESP32-CAM consume mucha corriente al transmitir video. Si la pantalla de la PDA se vuelve loca o parpadea cuando la cámara está encendida, usa fuentes de poder separadas o un capacitor de 1000uF entre VCC y GND.
3. **Calibración**: Al cargar el código por primera vez, el sistema pedirá tocar los puntos en pantalla. Esto es necesario para que los pines del Touch coincidan con los botones de la interfaz.

---
**Proyecto AricaTux PDA - Código Abierto para la comunidad de YouTube.**
