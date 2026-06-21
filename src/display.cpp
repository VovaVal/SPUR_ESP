#include "globals.h"


void drawMainMenu() {
    oled.clear();
    oled.home();

    oled.print("Файлов: ");
    oled.print(fileCount);

    // oled.print("  Битые: ");
    // oled.print(badCount);

    oled.line(0, 10, MAX_WIDTH, 10);

    oled.setCursor(0, START_Y_FILE);

    int i = 2, sidx = (cursor < 6 ? 0 : cursor - 5);

    su::TextParser p(fileNames, '/');
    while (p.parse()) {
        if (p.index() > sidx) {
            oled.setCursor(6, i++);
            if (p.lengthUnicode() > 20) {
                String file_name = p.substringUnicode(0, 20);
                oled.print(file_name);
            } else {
                oled.print(p);
            }
        }


        if (p.index() == cursor + 1) {
            selectedFile = String(p);
        }

        if (p.index() > sidx + 5) {
            break;
        }

        yield();
    }

    int cp = constrain(cursor, 0, 5) + 2;
    oled.setCursor(0, cp);
    
    if (fileCount > 0) {
        oled.print(">");
    }

    checkBattery();
    drawBattery();

    oled.update();
}


void drawSTAMenu() {
    oled.clear();
    oled.home();

    oled.print("РЕЖИМ STA");
    oled.line(0, 10, MAX_WIDTH, 10);
    oled.setCursor(0, 2);
    oled.print("Сеть: ");
    oled.print(settings.staSsid);
    oled.setCursor(0, 4);
    oled.print("Локал.IP:");
    oled.print(WiFi.localIP());

    checkBattery();
    drawBattery();

    oled.update();
}


void drawApMenu() {
    oled.clear();
    oled.home();

    oled.print("РЕЖИМ AP");
    oled.line(0, 10, MAX_WIDTH, 10);

    oled.setCursor(0, 2);
    oled.print(F("Сеть: "));
    oled.print(settings.apSsid);    // Выводим имя сети
    oled.setCursor(0, 4);
    oled.print(F("Ключ: "));
    oled.print(settings.apPass);    // Выводим пароль
    oled.setCursor(0, 6);
    oled.print(F("Локал.IP: "));
    oled.print(F("192.168.4.1"));   // Выводим IP

    checkBattery();
    drawBattery();
  
    oled.update();       
}


void checkBattery() {
    batMv = ESP.getVcc();
}


byte charge{};
void drawBattery() {
    charge = constrain(
        map(batMv, VBAT_EMPTY_MV, VBAT_FULL_MV, 0, 12), 0, 12
    );

    oled.setCursorXY(110, 0);
    oled.drawByte(0b00111100);
    oled.drawByte(0b00111100);
    oled.drawByte(0b11111111); 

    for (int i = 0; i < 12; i++) {
        if (i < 12 - charge) {
            oled.drawByte(0b10000001);
        } else {
            oled.drawByte(0b11111111);
        }
    }
    oled.drawByte(0b11111111);
}


void drawReadError() {
    oled.clear();

    oled.setScale(2);
    oled.setCursorXY(28, 8);
    oled.print("ОШИБКА");
    oled.setCursorXY(28, 24);
    oled.print("ЧТЕНИЯ");
    oled.setCursorXY(28, 40);
    oled.print("ФАЙЛА");

    oled.setScale(1);
    oled.update();
    delay(2000);
}


byte cur{0};
void EnterSettings() {
    cur = 0;

    while (!LittleFS.begin()) {
        LittleFS.format();
    }

    SettingsMenu();

    while (true) {
        ok.tick();
        up.tick();
        down.tick();
        data.tick();

        if (up.click()) {
            cur = constrain(cur - 1, 0, 1);
            SettingsMenu();
        } else if (down.click()) {
            cur = constrain(cur + 1, 0, 1);
            SettingsMenu();
        }

        if (ok.click()) {
            switch (cur)
            {
            case 0:
                LittleFS.format();
                drawFormatNotify();
                SettingsMenu();
                break;
            
            case 1:
                uiTimer = millis();
                LittleFS.end();
                checkFileSystem();
                drawMainMenu();
                return;
            }
        }

        yield();
    }
}


void SettingsMenu() {
    oled.clear();
    oled.home();

    oled.print("НАСТРОЙКИ");
    oled.line(0, 10, MAX_WIDTH, 10);
    checkBattery();
    drawBattery();

    oled.setCursor(6, 2);
    oled.print("ФОРМАТИРОВАНИЕ");
    
    oled.setCursor(6, 3);
    oled.print("ВЫХОД");

    oled.setCursor(0, cur + 2);
    oled.print(">");

    oled.update();
}


void drawFormatNotify(void) {
  oled.clear();
  oled.setScale(2);

  oled.setCursorXY(34, 16);
  oled.print(F("файлы"));
  oled.setCursorXY(22, 32);
  oled.print(F("удалены"));
  oled.setScale(1);

  oled.update();
  delay(1500);
}