/**
   BasicHTTPSClient.ino

    Created on: 20.08.2018
    
    Mod by https://www.youtube.com/lyovav
    25.01.2022

*/

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClientSecureBearSSL.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);
ESP8266WiFiMulti WiFiMulti;


// 1. Міняєємо на назву Вашого  WiFi 
const char* SSID = "мій вайфай";
// 2. Міняєємо на  Ваш WiFi password
const char* PASSWORD = "пароль від вайфаю";


void setup() {
  lcd.init();                      // initialize the lcd
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  Serial.begin(115200);
  // Serial.setDebugOutput(true);



  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(SSID, PASSWORD);
}

void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    client->setInsecure();
    // Or, if you happy to ignore the SSL certificate, then use the following line instead:
    // client->setInsecure();

    HTTPClient https;
// 3. Міняємо USER на Ваш нікнейм веб гаманця  в Duino-Coin username

    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, "https://server.duinocoin.com/users/USER")) {  // HTTPS

      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          Serial.println(payload);
          DynamicJsonDocument doc(5000);
          const char*json = payload.c_str();
          deserializeJson(doc, json);

          JsonObject obj = doc.as<JsonObject>();

          int miner = obj["result"]["miners"].size();
          float balance = obj["result"]["balance"]["balance"].as<float>();

          Serial.println(miner);

          lcd.setCursor(0, 1);
          lcd.print("workers:");
          lcd.setCursor(7, 1);
          lcd.print(miner);
          lcd.print("  ");
          lcd.setCursor(0, 0);
          lcd.print("balance:");
          lcd.setCursor(8, 0);
          lcd.print(balance);
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }
//обновлення 1 хвилина, меньше не рекомендую - можуть блоканути по ІР
  delay(60000);







}
