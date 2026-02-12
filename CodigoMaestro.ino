/*
 * PROJECT: AricaTux PDA (Master Open Source Edition)
 * AUTHOR: @ClaudioBravo-k4g
 * BOARD: ESP32 WROOM-32S
 * DATE: 2026
 */

#include <TFT_eSPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>

// --- HARDWARE & UI CONFIG ---
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite canvas = TFT_eSprite(&tft);
TFT_eSprite camFrame = TFT_eSprite(&tft);
WiFiUDP udp;

#define BUZZER_PIN 21
#define WM_BLUE    0x1975
#define WM_GREY    0xD6BA

// --- SYSTEM STATES ---
enum State { BOOT_LINUX, CALIBRATE, TODAY, START_MENU, TUXWORD, NAVIGATOR, CAMERA, TUXFS, INVADERS };
State currentState = BOOT_LINUX;
uint16_t calData[5];
String docText = "AricaTux > ";
int flashMode = 0; // 0:OFF, 1:AUTO, 2:ON

// --- SOUNDS ---
void beep(int f, int d) {
  digitalWrite(BUZZER_PIN, HIGH); delay(d); digitalWrite(BUZZER_PIN, LOW);
}

// --- LINUX BOOT SEQUENCE ---
void linuxBoot() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_YELLOW);
  for(int i=0; i<3; i++) {
    tft.drawString(" (o_ ", 15 + (i*40), 10);
    tft.drawString(" //\\ ", 15, 20 + (i*0)); // Simplified for speed
    tft.drawString(" V_/_", 15 + (i*40), 30);
  }
  
  tft.setTextColor(TFT_WHITE);
  const char* logs[] = {"CPU: Dual Core OK", "RAM: 512KB VFS", "NET: WiFi Stack", "GUI: X-Server"};
  for(int i=0; i<4; i++) {
    int y = 50 + (i*15);
    tft.setCursor(0, y);
    tft.print("["); tft.setTextColor(TFT_RED); tft.print("***");
    tft.setTextColor(TFT_WHITE); tft.print("] "); tft.print(logs[i]);
    delay(400);
    tft.fillRect(0, y, 40, 10, TFT_BLACK);
    tft.setCursor(0, y);
    tft.print("[ "); tft.setTextColor(TFT_GREEN); tft.print("OK");
    tft.setTextColor(TFT_WHITE); tft.println(" ]");
  }
  delay(800);
  currentState = CALIBRATE;
}

// --- UI COMPONENTS ---
void drawTaskbar(String title) {
  canvas.fillRect(0, 0, 128, 18, WM_BLUE);
  canvas.setTextColor(TFT_WHITE);
  canvas.drawString(title, 4, 3, 1);
  if (WiFi.status() == WL_CONNECTED) canvas.fillCircle(105, 9, 3, 0x07E0);
  else canvas.fillCircle(105, 9, 3, TFT_RED);
  canvas.fillRect(112, 2, 14, 14, TFT_RED);
  canvas.drawString("X", 116, 3, 1);
}

void drawStartMenu() {
  canvas.fillRect(2, 20, 104, 134, 0); 
  canvas.fillRect(0, 18, 102, 132, TFT_WHITE);
  canvas.drawRect(0, 18, 102, 132, WM_BLUE);
  canvas.setTextColor(TFT_BLACK);
  String apps[] = {"TuxWord", "Internet", "TuxCam", "Files", "Invaders"};
  for(int i=0; i<5; i++) {
    canvas.drawString(String(i+1) + ". " + apps[i], 10, 28 + (i*25), 2);
    canvas.drawFastHLine(5, 48 + (i*25), 90, WM_GREY);
  }
}

// --- MAIN ARDUINO SETUP ---
void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  tft.begin();
  tft.setRotation(0);
  canvas.createSprite(128, 160);
  camFrame.createSprite(96, 96);
  WiFi.begin("AricaTux-Cam");
  udp.begin(4444);
}

// --- MAIN LOOP ---
void loop() {
  if (currentState == BOOT_LINUX) { linuxBoot(); return; }
  
  if (currentState == CALIBRATE) {
    tft.fillScreen(TFT_BLACK);
    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);
    currentState = TODAY;
    return;
  }

  uint16_t x, y;
  bool pressed = tft.getTouch(&x, &y);

  if (pressed) {
    if (x > 105 && y < 25) { currentState = TODAY; delay(200); }
    else if (x < 45 && y < 25) { currentState = (currentState == START_MENU) ? TODAY : START_MENU; delay(300); }
    else if (currentState == START_MENU && x < 105) {
       if (y > 20 && y < 45) currentState = TUXWORD;
       else if (y > 70 && y < 95) currentState = CAMERA;
       else if (y > 120) currentState = INVADERS;
       delay(200);
    }
    // Flash Toggle in Camera Mode
    if (currentState == CAMERA && y > 135) {
      flashMode = (flashMode + 1) % 3;
      udp.beginPacket("192.168.4.1", 4444);
      udp.write('0' + flashMode);
      udp.endPacket();
      delay(200);
    }
  }

  canvas.fillSprite(TFT_WHITE);

  switch(currentState) {
    case TODAY:
      drawTaskbar("Start");
      canvas.setTextColor(TFT_BLACK);
      canvas.drawCentreString("AricaTux PDA", 64, 60, 2);
      canvas.drawCentreString("Kernel 6.1 Ready", 64, 85, 1);
      break;

    case CAMERA: {
      drawTaskbar("TuxCam");
      canvas.pushSprite(0, 0);
      int packetSize = udp.parsePacket();
      if (packetSize) {
        static int px = 0, py = 0;
        uint8_t buf[1024];
        int len = udp.read(buf, 1024);
        for(int i=0; i<len; i+=2) {
          uint16_t c = (buf[i] << 8) | buf[i+1];
          camFrame.drawPixel(px, py, c);
          px++; if(px >= 96) { px = 0; py++; }
          if(py >= 96) { py = 0; camFrame.pushSprite(16, 35); beep(2000, 5); }
        }
      }
      tft.fillRect(10, 140, 108, 18, WM_BLUE);
      tft.setTextColor(TFT_WHITE);
      String fTxt = (flashMode == 0) ? "FLASH: OFF" : (flashMode == 1) ? "FLASH: AUTO" : "FLASH: ON";
      tft.drawCentreString(fTxt, 64, 143, 1);
      break;
    }

    case TUXWORD:
      drawTaskbar("TuxWord");
      canvas.drawString(docText + "|", 5, 30);
      canvas.fillRect(0, 110, 128, 50, WM_GREY);
      break;

    case INVADERS:
      drawTaskbar("Invaders");
      canvas.fillRect(0, 18, 128, 142, TFT_BLACK);
      canvas.setTextColor(TFT_GREEN);
      canvas.drawString("H  H  H", 45, 60, 2);
      break;
  }

  if (currentState == START_MENU) drawStartMenu();
  if (currentState != CAMERA) canvas.pushSprite(0, 0);
  yield();
}
