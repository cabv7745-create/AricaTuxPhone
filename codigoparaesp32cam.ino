/*
 * PROJECT: AricaTux PDA - Camera Transmitter (Open Source)
 * AUTHOR: @ClaudioBravo-k4g
 * BOARD: ESP32-CAM (AI-Thinker)
 * DATE: 2026
 * * DESCRIPTION: Captures 96x96 RGB565 frames and sends them via UDP 
 * to the WROOM-32S PDA. Includes Smart Flash control.
 */

#include "esp_camera.h"
#include <WiFi.h>
#include <WiFiUdp.h>

// --- PINES AI-THINKER (No cambiar) ---
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
#define LED_FLASH          4 

// --- CONFIGURACIÓN DE RED ---
const char* ssid = "AricaTux-Cam";
const char* pda_ip = "192.168.4.2"; // IP estática de la PDA
const int port = 4444;
WiFiUDP udp;

int flashMode = 0; // 0: OFF, 1: AUTO, 2: ON

void setup() {
  Serial.begin(115200);
  pinMode(LED_FLASH, OUTPUT);
  digitalWrite(LED_FLASH, LOW); // Flash apagado al iniciar

  // Configuración de la Cámara
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_RGB565; // Formato de color para la PDA
  config.frame_size = FRAMESIZE_96X96;    // Tamaño optimizado para RAM interna
  config.fb_count = 1;

  // Inicializar Cámara
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Error al iniciar cámara: 0x%x", err);
    return;
  }

  // Crear Punto de Acceso WiFi
  WiFi.softAP(ssid);
  udp.begin(port);
  
  Serial.println("AricaTux-Cam lista!");
  Serial.print("IP del AP: "); Serial.println(WiFi.softAPIP());
}

void loop() {
  // 1. Escuchar comandos de la PDA (Control de Flash)
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char cmd = udp.read();
    if (cmd == '0') flashMode = 0;
    if (cmd == '1') flashMode = 1;
    if (cmd == '2') flashMode = 2;
    Serial.printf("Nuevo modo Flash: %d\n", flashMode);
  }

  // 2. Lógica de Brillo para Modo Automático
  sensor_t * s = esp_camera_sensor_get();
  // AEC (Auto Exposure Control) nos da una pista de la luz
  int exposure = s->status.aec_value; 

  // 3. Capturar Fotograma
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) return;

  // Determinar si encender el flash
  bool triggerFlash = false;
  if (flashMode == 2) triggerFlash = true;
  if (flashMode == 1 && exposure > 1000) triggerFlash = true; // Umbral de oscuridad

  if (triggerFlash) digitalWrite(LED_FLASH, HIGH);

  // 4. Enviar imagen por paquetes UDP (Fragmentación)
  int totalLen = fb->len;
  int sent = 0;
  int chunkSize = 1024; // Tamaño de paquete UDP seguro

  while (sent < totalLen) {
    int toSend = (totalLen - sent < chunkSize) ? totalLen - sent : chunkSize;
    udp.beginPacket(pda_ip, port);
    udp.write(&(fb->buf[sent]), toSend);
    udp.endPacket();
    sent += toSend;
  }
  
  if (triggerFlash) {
    delay(50); // Mantener flash para la captura
    digitalWrite(LED_FLASH, LOW);
  }

  // 5. Liberar memoria y esperar al siguiente frame (Stop-Motion)
  esp_camera_fb_return(fb);
  delay(500); // 2 cuadros por segundo (ajustable)
}
