# AricaTux PDA - Edición de Código Abierto 🐧

¡Bienvenido al proyecto **AricaTux PDA**! Este es un sistema operativo portátil personalizado, diseñado para el **ESP32 WROOM-32S** con pantalla táctil resistiva.

## ✨ Características
- 🐧 **Arranque del Kernel de Linux**: Simulación auténtica con pingüinos Tux y registros de estado [OK].
- 🎯 **Autocalibración**: Calibración táctil obligatoria para una alta precisión.
- 📱 **Interfaz gráfica**: Menú de Inicio y barra de tareas inspirados en Windows Mobile.
- 📸 **Cámara WiFi Stop-Motion**: Recibe vídeo en tiempo real de una ESP32-CAM vía UDP.
- 🎮 **Aplicaciones preinstaladas**: Procesador de textos, Explorador de archivos y clones de Space Invaders.

## 🛠️ Requisitos de hardware
- ESP32 WROOM-32S
- Pantalla TFT de 2,4" o 2,8" (ILI9341) táctil
- ESP32-CAM (para el módulo de cámara)

## 🚀 Cómo instalar
1. Instala la biblioteca `TFT_eSPI` en tu IDE de Arduino.
2. Sube el código `.ino` de este repositorio a tu ESP32 WROOM.
3. Enciende el ESP32-CAM (asegúrate de que esté transmitiendo "AricaTux-Cam").
4. Sigue la calibración en pantalla.

**¡Suscríbete al canal para estar al día!**
