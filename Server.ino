#include <ArduinoJson.h>

#include "GUI.hpp"
#include "Net.hpp"

const int MAX_TOKENS = 10;

const int LOOP_TIME = 1000;
int LastTime = 0;

enum class State {
  SETUP = 0,
  MONITOR = 2
};
State AppState = State::SETUP;

Net::NetworkSystem NetSystem(
  Net::NetworkSystem::Options{
    .I_IsDebug = true,
    .I_PORT = 80,

    .I_AP_LOCAL_IP = IPAddress(192,168,10,1),
    .I_AP_GATEWAY = IPAddress(192,168,10,1),
    .I_AP_SUBNET = IPAddress(255,255,255,0),

    .I_AP_SSID = "WiFi",
    .I_AP_PASSWORD = "Password"
  }
);

void setup() {
  Serial.begin(115200);
}

void loop() {

  switch (AppState) {
    case State::SETUP: {
      Net::Initialize();
      NetSystem.WiFiInitialize();

        AppState = State::MONITOR;
        return;

      while (millis() < 100000) {
        Net::Server.handleClient();

        Serial.println(WiFi.softAPIP());
        Serial.println(WiFi.status());

        Serial.print("Stations connected: ");
        Serial.println(WiFi.softAPgetStationNum());
      }

      AppState = State::MONITOR;
    }
    break;

    case State::MONITOR: {
      if (millis() - LastTime >= LOOP_TIME) {
        LastTime = millis();
      } else return;

      Net::NetworkClient.setCACertBundle(x509_crt_bundle, x509_crt_bundle_len);
      //Net::NetworkClient.setInsecure();

      Serial.println("\nStarting connection to server...");

      if (Net::HTTPSClient.begin(Net::NetworkClient, Net::API_REQUEST)) {
        Serial.println("Connected to server!");

        int ReturnedHTTPCode = Net::HTTPSClient.GET();
        if (ReturnedHTTPCode > 0) {
          String RecievedData = Net::HTTPSClient.getString();

          Serial.printf("[HTTPS] GET code: %d\n", ReturnedHTTPCode);
          Serial.printf("[HTTPS] GET payload: ");

          JsonDocument DataDecoder;
          DeserializationError HasDecodeFailed = deserializeJson(DataDecoder, RecievedData);

          if (HasDecodeFailed) {
            Serial.print("JSON deserialization failed: ");
            Serial.println(HasDecodeFailed.c_str());
            return;
          }

          String DataType = DataDecoder[0]["type"];
          if (DataType == "sgv") {
            int SGV = DataDecoder[0]["sgv"];
            Serial.print(double(SGV) * (10000.0 / 180155.9));
            Serial.println("mmol/L");
          }
          if (DataType == "mbg") {
            int MBG = DataDecoder[0]["sgv"];
            Serial.println(double(MBG) * (10000.0 / 180155.9));
            Serial.println("mmol/L");
          }
        } 
        else Serial.printf("[HTTPS] GET... failed, error: %s\n", Net::HTTPSClient.errorToString(ReturnedHTTPCode).c_str());
      }
      Net::HTTPSClient.end();
    }
    break;
  }
}
