#include <WiFi.h>
#include <WebServer.h>
#include "EmonLib.h"
#include "config.h"

WebServer httpServer(80);
EnergyMonitor emon1;

void handle_request() {
    double amps = emon1.calcIrms(1480);
    Serial.println(amps);
    
    String HTMLData = "current{label=\"phase1\"} " + String(amps);
    httpServer.send(200, "text/html", HTMLData);
    
    digitalWrite(INDICATE_PIN, HIGH);
    delay(10);
    digitalWrite(INDICATE_PIN, LOW);
} 

void setup() {
    analogReadResolution(12);

    pinMode(INDICATE_PIN, OUTPUT);
    Serial.begin(115200);
    while (!Serial) {}
    Serial.println("Tring to connect to Wifi SSID: ");
    Serial.println(ssid);
  
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
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
}
