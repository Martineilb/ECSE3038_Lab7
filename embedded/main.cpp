#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <env.h>

#define ONBOARD_LED 2

int getWaterLevel();
int value = 0;
const int potPin = 34;

void setup()
{
    Serial.begin(115200);
    pinMode(ONBOARD_LED, OUTPUT);

    WiFi.begin(Wifi_ssid, Wifi_passwd);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print("*");
    }

    Serial.println("");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

void loop()
{
    value = getWaterLevel();

    if (value >= 80 && value <= 100)
    {
        digitalWrite(ONBOARD_LED, HIGH);
    }
    else
    {
        digitalWrite(ONBOARD_LED, LOW);
    }

    HTTPClient http;

    http.begin("localhost:3000/tank");

    http.addHeader("Content-Type", "application/json");
    char output[128];
    const size_t CAPACITY = JSON_OBJECT_SIZE(4);
    StaticJsonDocument<CAPACITY> doc;

    doc["tank_id"].set(WiFi.macAddress());
    doc["level_of_water"].set(value);

    serializeJson(doc, output);
    int httpResponseCode = http.POST(String(output));
    String payload = "{}";
    if (httpResponseCode > 0)
    {
        payload = http.getString();
    }
    else
    {
        Serial.print("Failure: ");
        Serial.println(httpResponseCode);
    }

    Serial.println(payload);
    delay(5000);
    http.end();
}

int getWaterLevel()
{
    float val = analogRead(potPin);
    val = round(((val / 4095) * 5) / 0.01);
    return val;
}