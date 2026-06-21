#include "globals.h"

void build() {                     // Билд страницы
  GP.BUILD_BEGIN(400);             // Ширина колонок
  GP.THEME(GP_DARK);               // Темная тема
  GP.PAGE_TITLE("Wi-Fi Reader");   // Обзываем титл
  GP.FORM_BEGIN("/cfg");           // Начало формы
  GP.GRID_RESPONSIVE(600);         // Отключение респонза при узком экране
  M_BLOCK(                         // Общий блок-колонка для WiFi
    GP.SUBMIT("Сохранить");        // Кнопка отправки формы
    M_BLOCK_TAB(                   // Конфиг для AP режима -> текстбоксы (логин + пароль)
      "Точка доступа",             // Имя + тип DIV
      GP.TEXT("apSsid", "Имя сети", settings.apSsid, "", 20);
      GP.BREAK();
      GP.TEXT("apPass", "Пароль", settings.apPass, "", 20); GP.BREAK(););
    M_BLOCK_TAB(                   // Конфиг для STA режима -> текстбоксы (логин + пароль)
      "Подключение к сети",        // Имя + тип DIV
      GP.TEXT("staSsid", "Имя сети", settings.staSsid, "", 20);
      GP.BREAK();
      GP.TEXT("staPass", "Пароль", settings.staPass, "", 20); GP.BREAK();
      M_BOX(GP_CENTER, GP.LABEL("Автоподключение"); GP.SWITCH("staEn", settings.staModeEn);););
    M_BLOCK_TAB(                   // Другие настройки
      "Другое",
      M_BOX(GP_CENTER, GP.LABEL("Яркость"); GP.SLIDER("con", settings.dispContrast, 10, 100););););
  GP.FORM_END();                 // <- Конец формы (костыль)
  M_BLOCK_TAB(                   // Блок с OTA-апдейтом
    "Обновление прошивки",       // Имя + тип DIV
    GP.OTA_FIRMWARE();           // Кнопка с OTA начинкой
  );
  M_BLOCK_TAB(                   // Блок с файловым менеджером
    "Файловая система",          // Имя + тип DIV
    GP.FILE_UPLOAD("file_upl");  // Кнопка для загрузки файла
    GP.FILE_MANAGER(&LittleFS);  // Файловый менеджер
  );
  GP.BUILD_END();                  // Конец билда страницы
}

void action(GyverPortal& p) {      // Подсос значений со страницы
  if (p.form("/cfg")) {            // Если есть сабмит формы - копируем все в переменные
    p.copyStr("apSsid", settings.apSsid);
    p.copyStr("apPass", settings.apPass);
    p.copyStr("staSsid", settings.staSsid);
    p.copyStr("staPass", settings.staPass);
    p.copyBool("staEn", settings.staModeEn);
    p.copyInt("con", settings.dispContrast);
    byte con = map(settings.dispContrast, 10, 100, 1, 255);
    oled.setContrast(con);            // Тут же задаем яркость оледа

    if (strlen(settings.apSsid) < 1)  strcpy(settings.apSsid, AP_DEFAULT_SSID);   // Проверка на пустое имя сети AP
    if (strlen(settings.apPass) < 8)  strcpy(settings.apPass, AP_DEFAULT_PASS);   // Проверка на пустой пасс AP

    if (settings.staModeEn) {                                                 // Если включен коннект
      if (strlen(settings.staSsid) < 1 || strlen(settings.staPass) < 8) {         // Проверка имени и пасса
        settings.staModeEn = false;                                           // Битое имя/пасс - выключаем коннект
      }
    }

    uint8_t contrast = map(settings.dispContrast, 10, 100, 1, 255);
    oled.setContrast(contrast);   // Тут же задаем яркость оледа
    data.update();                // Записать. Дальше оно само
  }
}