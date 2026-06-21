#include "globals.h"


void enterToWifi() {
    oled.clear();
    oled.home();

    oled.print("WiFi МЕНЮ");
    oled.line(0, 10, MAX_WIDTH, 10);

    if (settings.staModeEn) {
        oled.setCursor(0, 2);
        oled.print("Подключение");
        oled.update();
        
        WiFi.mode(WIFI_STA);
        WiFi.begin(settings.staSsid, settings.staPass);
        oled.setCursor(66, 2);
        for (int i = 0; i < 10; i++) {
            if (WiFi.status() != WL_CONNECTED) {
                oled.print(".");
                oled.update();

                delay(1000);
            } else {
                drawSTAMenu();

                ui.attachBuild(build);
                ui.attach(action);
                ui.start();
                ui.enableOTA();

                while (true) {
                    ok.tick();
                    up.tick();
                    down.tick();
                    data.tick();

                    if (up.click() || up.hold()) {
                        uiTimer = millis();

                        settings.dispContrast = constrain(settings.dispContrast + 10, 10, 100);
                        byte con = map(settings.dispContrast, 10, 100, 1, 255);
                        oled.setContrast(con);
                    } else if (down.click() || down.hold()) {
                        uiTimer = millis();

                        settings.dispContrast = constrain(settings.dispContrast - 10, 10, 100);
                        byte con = map(settings.dispContrast, 10, 100, 1, 255);
                        oled.setContrast(con);
                    } else if ((ok.hold() || ok.click()) || (millis() - uiTimer) >= WIFI_TIMEOUT_S * 1000) {
                        uiTimer = millis();

                        checkFileSystem();
                        drawMainMenu();

                        data.update();
                        WiFi.mode(WIFI_OFF);

                        return;
                    } else if (ok.hasClicks(2)) {
                        uiTimer = millis();

                        settings.staModeEn = false;

                        checkFileSystem();
                        drawMainMenu();

                        data.update();
                        WiFi.mode(WIFI_OFF);
                    }

                    if (millis() - uiTimer >= 5000) {
                        uiTimer = millis();

                        drawSTAMenu();
                    }

                    ui.tick();
                    yield();
                }
            }
        }
    } else {
        WiFi.mode(WIFI_AP);
        WiFi.softAP(settings.apSsid, settings.apPass); 
        drawApMenu();
        
        ui.attachBuild(build);
        ui.attach(action);
        ui.start();
        ui.enableOTA();

        while (true) {
            ok.tick();
            up.tick();
            down.tick();
            data.tick();

            if (up.click() || up.hold()) {
                uiTimer = millis();

                settings.dispContrast = constrain(settings.dispContrast + 10, 10, 100);
                byte con = map(settings.dispContrast, 10, 100, 1, 255);
                oled.setContrast(con);
            } else if (down.click() || down.hold()) {
                uiTimer = millis();

                settings.dispContrast = constrain(settings.dispContrast - 10, 10, 100);
                byte con = map(settings.dispContrast, 10, 100, 1, 255);
                oled.setContrast(con);
            } else if ((ok.click() || ok.hold()) || (millis() - uiTimer) >= WIFI_TIMEOUT_S * 1000) {
                uiTimer = millis();

                checkFileSystem();
                drawMainMenu(); 

                data.update();
                WiFi.softAPdisconnect();
                WiFi.mode(WIFI_OFF);

                return;
            }

            if (millis() - uiTimer >= 5000) {
                uiTimer = millis();

                drawApMenu();
            }

            ui.tick();
            yield();
        }
    }
}