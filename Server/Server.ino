#include <ArduinoJson.h>

#include "GUI.hpp"
#include "Net.hpp"

const int MAX_TOKENS = 10;

const int LOOP_TIME = 1000;
int LastTime = 0;

int debugTime = 0;

enum class State {
  SETUP = 0,
  MONITOR = 2
};
State AppState = State::SETUP;

Web::WebSystem WebSys(
  Web::WebSystem::Options{
    .I_IsDebug = true
  }
);

Net::NetworkSystem NetSys(
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

    .I_AP_CHANNEL = 6,
    .I_AP_NETWORK_NAME_IS_HIDDEN = 0,
    .I_AP_MAX_DEVICES = 4,

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
  delay(1);

  switch (AppState) {
    case State::SETUP: {
      NetSys.NS_SITE_Setup();
      NetSys.WiFiInitialize(WebSys);

      while (true) {
        NetSys.AP_Tick(1);
      }

      AppState = State::MONITOR;
    }
    break;

    case State::MONITOR: {
      if (millis() - LastTime >= LOOP_TIME) {
        LastTime = millis();
      } else return;

      NetSys.STA_Tick();

      if (NetSys.GetPacket().Code > 0) {
        JsonDocument DataDecoder;
        DeserializationError HasDecodeFailed = deserializeJson(DataDecoder, NetSys.GetPacket().Payload);

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
}
