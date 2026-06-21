#include "globals.h"


void checkFileSystem(void) {           // Проверка и индексация файловой системы
  Dir root = LittleFS.openDir("/");    // Открываем директорию (корень)
  fileCount = badCount = 0;            // Обнуляем счетчики файлов
  fileNames = "";                      // Обнуляем список имен
  for (int i = 0; root.next(); i++) {  // Шагаем по директории
    yield();                           // Внутренний поллинг
    File file = root.openFile("r");    // Открываем файл для чтения
    if (file) {                        // Если файл существует
      su::Text filename(file.name());  // Создаем строку с именем
      if ((filename.endsWith(".txt") || filename.endsWith(".itxt") || filename.endsWith(".h") || filename.endsWith(".jpg"))) {
        fileCount++;                   // Нормальный файл (Имя короткое, тип .txt / .itxt / .h / .jpg)
        fileNames += "/";              // + /
        fileNames += root.fileName();  // + Имя файла
      } else if (!filename.endsWith(".dat")) badCount++;     // Битый
    } else badCount++;                                       // Битый
    file.close();                                            // Закрываем файл
  }
}


long pagePos[100];
int currentPage = 0;
void drawPage(File& file) {
    if (!file.available()) {
        return;
    }

    oled.clear();
    oled.home();
    oled.autoPrintln(true);

    pagePos[currentPage] = file.position();

    while (file.available() && !oled.isEnd()) {
        char c = file.read();
        oled.print(c);
    }

    oled.update();
}


String fn{};
void readTXTFile() {
    fn = ("/" + selectedFile);
    File file = LittleFS.open(fn, "r");

    if (!file) {
        drawReadError();

        checkFileSystem();
        drawMainMenu();

        file.close();
        return;
    }

    currentPage = 0;

    drawPage(file);
    while (true) {
        up.tick();
        down.tick();
        ok.tick();
        
        if (ok.click()) {
            uiTimer = millis();

            drawMainMenu();

            file.close();
            return;
        } else if (up.click()) {
            uiTimer = millis();

            if (currentPage > 0) {
                currentPage--;

                file.seek(pagePos[currentPage]);
                drawPage(file);
            }
        } else if (down.click()) {
            uiTimer = millis();

            if (file.available()) {
                currentPage++;
                drawPage(file);
            }
        }

        yield();
    }
}


bool INVERT_IMG{0};
void enterToReadBmpFile(void) {
  String fn = ("/" + selectedFile);    // Собираем путь до файла
  File file = LittleFS.open(fn, "r");  // Открываем файл
  if (!file) {                         // Если сам файл не порядке
    Serial.println("Error! File is bad");
    drawReadError();

    checkFileSystem();  // Чекаем файловую систему
    drawMainMenu();     // Рисуем главное меню

    file.close();       // Закрываем файл
    return;             // Выходим
  }

  uint8_t *img = new uint8_t[128 * 64];
  if (parseItxt(img, file)) {
    Serial.println("Error! Error reading");
    drawReadError();     // Выводим ошибку чтения
    delete[] img;        // Выгружаем буфер
    uiTimer = millis();  // Сбрасываем таймер дисплея
    drawMainMenu();      // Рисуем главное меню
    file.close();        // Закрываем файл
    return;              // Выходим
  }

  oled.clear();                             // Чистим олед
  oled.drawBmpFromRam(0, 0, img, 128, 64);  // Выводим картинку
  oled.update();                            // Обновляем олед
  file.close();                             // Закрываем файл

  while (1) {              // Бесконечный цикл
    ok.tick();             // Опрос кнопки
    down.tick();           // Опрос кнопки
    if (ok.click()) {      // Если ок нажат
      uiTimer = millis();  // Сбрасываем таймер дисплея
      drawMainMenu();      // Рисуем главное меню
      delete[] img;        // Выгружаем буфер
      return;              // Выходим
    }
    if (down.click()) {    // Если нажали вниз
      File file = LittleFS.open(fn, "r");  // Открываем файл
      if (!file) {                         // Если сам файл не порядке
        drawReadError();
        checkFileSystem();   // Чекаем файловую систему
        file.close();        // Закрываем файл
        delete[] img;        // Выгружаем буфер
        uiTimer = millis();  // Сбрасываем таймер дисплея
        drawMainMenu();      // Рисуем главное меню
        return;              // Выходим
      }
      oled.clear();             // Залить чёрным
      INVERT_IMG = !INVERT_IMG; // Инвертировать
      if (parseItxt(img, file)) {
        drawReadError();     // Выводим ошибку чтения
        delete[] img;        // Выгружаем буфер
        uiTimer = millis();  // Сбрасываем таймер дисплея
        drawMainMenu();      // Рисуем главное меню
        file.close();        // Закрываем файл
        return;              // Выходим
      }
      oled.drawBmpFromRam(0, 0, img, 128, 64);  // Выводим картинку
      oled.update();       // Обновить
      file.close();        // Закрываем файл
    }
    yield();  // Внутренний поллинг ESP
  }
}


uint8_t parseItxt(uint8_t *img, File file) {
  int imgLen = 0;                               // Длина + индекс для массива
  memset(img, 0, 1024);                         // Чистим буфер

  while (file.read() != '{') {                  // Ищем начало массива '{'
    if (!file.available()) return 1;            // Если не нашли - ошибка
    yield();                                    // Внутренний поллинг ESP
  }

  while (file.available()) {                    // Пока файл не кончился
    String line = file.readStringUntil('\n');   // Читаем по строке
    su::TextParser p(line, ',');                // Готовим парсер
    while (p.parse()) {                         // Парсим по ','
      uint8_t val = p.trim().toInt32HEX();      // Вытаскиваем байт
      if (INVERT_IMG) val = ~val;               // Если надо - инвертировать
      img[imgLen] = val;                        // Записать
      if (++imgLen >= 1023) return 0;           // Как только насобирали 1кб - выходим праздновать
      yield();                                  // Внутренний поллинг ESP
    } yield();                                  // На всякий случай еще и для внешнего цикла
  }

  return 1;                                     // не насобирали - ошибка
}