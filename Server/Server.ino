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
    .I_STA_IsInsecure = false,
    .I_HTTP_PORT = 80,
    .I_DNS_PORT = 53,

    .I_AP_LOCAL_IP = IPAddress(192,168,10,1),
    .I_AP_GATEWAY = IPAddress(192,168,10,1),
    .I_AP_SUBNET = IPAddress(255,255,255,0),

    .I_STA_SSID = "Cleb_Club",
    .I_STA_PASSWORD = "clebclubwifi",

    .I_AP_SSID = "ESP32_AP",
    .I_AP_PASSWORD = "espPassword",

    .I_API_SITE = "https://37741.cgm.bcdiabetes.ca/",
    .I_API_SECRET = "XIO-52361047",

    .I_Certificates = x509_crt_bundle,
    .I_CertificateSize = x509_crt_bundle_len
  }
);

void setup() {
  Serial.begin(115200);
}

void loop() {

  switch (AppState) {
    case State::SETUP: {
      NetSystem.NS_SITE_Setup();
      NetSystem.WiFiInitialize();

        //AppState = State::MONITOR;
        //return;

      while (true) {
        NetSystem.AP_Tick(20, 40);
        Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());

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

      NetSystem.STA_Tick();

      if (NetSystem.GetPacket().Code > 0) {
        JsonDocument DataDecoder;
        DeserializationError HasDecodeFailed = deserializeJson(DataDecoder, NetSystem.GetPacket().Payload);

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
    }
    break;
  }
  
  delay(2);
}
