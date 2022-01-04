#include <WiFi.h>
#include <WebServer.h>
#include "EmonLib.h"
#include "ESP.h"
#include "config.h"

WebServer httpServer(80);
EnergyMonitor emon1;
double amps = 0;

void handle_request() {
    amps = emon1.calcIrms(1480);
    Serial.println(amps);
    
    httpServer.send(200, "text/html", "current{label=\"phase1\"} " + String(amps));
    
    digitalWrite(INDICATE_PIN, HIGH);
    delay(10);
    digitalWrite(INDICATE_PIN, LOW);
    Serial.println(ESP.getFreeHeap());
} 

void setup() {
    analogReadResolution(12);

    pinMode(INDICATE_PIN, OUTPUT);
    Serial.begin(115200);
    while (!Serial) {}
    Serial.println("Tring to connect to Wifi SSID: ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("WiFi Connect Failed! Rebooting...");
        delay(1000);
        ESP.restart();
    }
    Serial.println("");
    Serial.println("WiFi connected successfully");
    Serial.print("Assigned IP: ");
    Serial.println(WiFi.localIP());

    emon1.current(ADC_INPUT, 30);
  
    httpServer.on("/", handle_request);
    httpServer.begin();
    Serial.println("HTTP server started");
    delay(100);
}

void loop() {
    httpServer.handleClient();
    delay(10);
}
