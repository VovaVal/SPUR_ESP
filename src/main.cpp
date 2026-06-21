#include <Arduino.h>
#include "globals.h"


void setup() {
  Serial.begin(9600);

  pinMode(UP_BTN_PIN, INPUT_PULLUP);
  pinMode(DWN_BTN_PIN, INPUT_PULLUP);
  pinMode(OK_BTN_PIN, INPUT_PULLUP);

  while (!LittleFS.begin()) {           // Инициализация файловой системы
    LittleFS.format();
  }
  data.read();

  oled.init(IIC_SDA_PIN, IIC_SCL_PIN);  // инициализируем дисплей
  Wire.setClock(400E3);  // частота работы I2C шины

  batMv = ESP.getVcc();

  oled.clear();
  oled.update();
  oled.autoPrintln(true);
  oled.setScale(1);

  checkFileSystem();
  drawMainMenu();
}

void loop() {
  ok.tick();
  up.tick();
  down.tick();
  data.tick();

  if (ok.click()) {  // Открываем файл
    uiTimer = millis();

    if (fileCount) {
      if (selectedFile.endsWith(".txt")) {
        readTXTFile();
      } else if (selectedFile.endsWith(".h") || selectedFile.endsWith(".itxt")) {
        enterToReadBmpFile();
      }
    }

  } else if (up.click()) {
    uiTimer = millis();

    cursor = constrain(cursor - 1, 0, fileCount - 1);
    drawMainMenu();
  } else if (down.click()) {
    uiTimer = millis();

    cursor = constrain(cursor + 1, 0, fileCount - 1);
    drawMainMenu();
  }

  if (ok.hold()) {
    uiTimer = millis();
    cursor = 0;

    enterToWifi();
  } else if (up.hold()) {
    uiTimer = millis();

    EnterSettings();
  }

  if (millis() - uiTimer >= 5000) {
    uiTimer = millis();

    drawMainMenu();
  }
}