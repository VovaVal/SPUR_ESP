#include "globals.h"


GyverPortal ui(&LittleFS);
Settings settings;
FileData data(&LittleFS, "/data.dat", EE_KEY, &settings, sizeof(settings));
GyverOLED<SSD1306_128x64> oled;
ADC_MODE(ADC_VCC);  // Режим работы АЦП - измерение VCC

uButton up(UP_BTN_PIN);
uButton down(DWN_BTN_PIN);
uButton ok(OK_BTN_PIN);

String selectedFile = "";  // Имя выбранной строки
String fileNames = "";     // Имена всех читаемых файлов
int16_t fileCount = 0;     // Количество читаемых файлов
int16_t badCount = 0;      // Количество битых файлов
int16_t cursor = 0;        // Указатель (курсор) меню
int16_t batMv = 3000;      // Напряжение питания ESP
uint32_t uiTimer = 0;      // Таймер таймаута дисплея
uint32_t batTimer = 0;     // Таймер опроса АКБ