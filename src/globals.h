#include <GyverPortal.h>
#include <LittleFS.h>
#include <FileData.h>
#include <GyverOLED_fix.h>
#include <StringUtils.h>
#include <uButton.h>

#define MAX_FILENAME_LEN  12            // Максимальная длина имени файла (не включая .txt/.itxt)
#define AP_DEFAULT_SSID   "Reader AP"   // Стандартное имя точки доступа ESP (До 20-ти символов)
#define AP_DEFAULT_PASS   "00000000"    // Стандартный пароль точки доступа ESP (До 20-ти символов)
#define STA_DEFAULT_SSID  ""            // Стандартное имя точки доступа роутера (До 20-ти символов)
#define STA_DEFAULT_PASS  ""            // Стандартный пароль точки доступа роутера (До 20-ти символов)
#define STA_CONNECT_EN    0             // 1/0 - вкл./выкл. подключение к роутеру
#define OLED_CONTRAST     100           // Яркость дисплея по умолчанию (%)
#define WIFI_TIMEOUT_S    300           // Таймаут на отключение Wi-Fi (С)
#define UP_BTN_PIN        14            // Номер GPIO для кнопки ВВЕРХ
#define OK_BTN_PIN        12            // Номер GPIO для кнопки ОК
#define DWN_BTN_PIN       13            // Номер GPIO для кнопки ВНИЗ
#define IIC_SDA_PIN       4             // Номер GPIO SDA дисплея
#define IIC_SCL_PIN       5             // Номер GPIO SCL дисплея
#define EE_KEY            'B'           // Ключ EEPROM (1 байт) - измени, чтобы сбросить настройки
#define VBAT_FULL_MV      3600          // Напряжение питания при заряженном аккуме в (мВ)
#define VBAT_EMPTY_MV     3000          // Напряжение питания при севшем аккуме в (мВ)
#define _EB_DEB           25            // Дебаунс кнопок (мс)
#define START_Y_FILE      15            // С какого y начинаем рисовать файлы

#define MAX_WIDTH 128
#define MAX_HEIGHT 64

// структура настроек сети контроллера
struct Settings{
  char apSsid[21] = AP_DEFAULT_SSID;    // Имя сети для AP режима по умолчанию
  char apPass[21] = AP_DEFAULT_PASS;    // Пароль сети для AP режима по умолчанию
  char staSsid[21] = STA_DEFAULT_SSID;  // Имя сети для STA режима по умолчанию
  char staPass[21] = STA_DEFAULT_PASS;  // Пароль сети для STA режима по умолчанию
  bool staModeEn = STA_CONNECT_EN;      // Подключаться к роутеру по умолчанию?
  int dispContrast = OLED_CONTRAST;     // Яркость оледа
};
extern Settings settings;

extern GyverPortal ui;  // Страница в вебе
extern FileData data;
extern GyverOLED<SSD1306_128x64> oled;

extern uButton up; 
extern uButton down;
extern uButton ok;


extern String selectedFile;  // Имя выбранной строки
extern String fileNames;  // Имена всех читаемых файлов
extern int16_t fileCount;  // Количество читаемых файлов
extern int16_t badCount;  // Количество битых файлов
extern int16_t cursor;  // Указатель (курсор) меню
extern int16_t batMv;  // Напряжение питания ESP
extern uint32_t uiTimer;  // Таймер таймаута дисплея
extern uint32_t batTimer;

void enterToWifi();
void build();
void action(GyverPortal& p);
void checkFileSystem(void);
void drawMainMenu();
void drawSTAMenu();
void drawApMenu();
void checkBattery();
void drawBattery();
void drawPage(File& file);
void readTXTFile();
void drawReadError();
void enterToReadBmpFile(void);
uint8_t parseItxt(uint8_t *img, File file);
void SettingsMenu();
void EnterSettings();
void drawFormatNotify(void);