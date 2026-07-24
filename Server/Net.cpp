#include <sys/_intsup.h>
#include "Net.hpp"

namespace Net {

  //char WiFi_SSID[] = "Cleb_Club";
  //char WiFi_PASSWORD[] = "clebclubwifi";

  //const String API_SITE = "https://37741.cgm.bcdiabetes.ca/";
  //const String API_SECRET = "XIO-52361047";
  //const String API_REQUEST = API_SITE + "api/v1/entries.json?count=1&api_secret=" + API_SECRET;

  //char AP_SSID[] = "ESP32_AP";
  //char AP_PASSWORD[] = "espPassword";

  void NetworkSystem::WiFiInitialize(Web::WebSystem& F_WebSystem) {
    if (M_IsDebug) {
      Serial.println("\nConnecting to network: ");
      Serial.print(M_STA_SSID);
      Serial.println("\n\n");
    }
    WiFi.disconnect(true);
    WiFi.persistent(false);
    WiFi.mode(WIFI_AP_STA);

    WiFi.softAPConfig(M_AP_LOCAL_IP, M_AP_GATEWAY_IP, M_AP_SUBNET_IP);
    WiFi.softAP(M_AP_SSID, M_AP_PASSWORD, M_AP_CHANNEL, M_AP_NETWORK_NAME_IS_HIDDEN, M_AP_MAX_DEVICES);

    WiFi.begin(M_STA_SSID, M_STA_PASSWORD);

    delay(1000);
    F_WebSystem.SetupSystem(&M_WebServer);
    M_WebServer.begin();
    unsigned int timer = 0;
    while (WiFi.softAPIP() == IPAddress(0, 0, 0, 0) || WiFi.status() != WL_CONNECTED) {
      esp_task_wdt_reset();
      delay(1);
      if (M_IsDebug && millis() - timer > 99) {
        Serial.print(". ");
        timer = millis();
      }
    }
    delay(2000);

    M_DNSServer.setErrorReplyCode(DNSReplyCode::NoError);
    M_DNSServer.start(M_DNS_PORT, "*", WiFi.softAPIP());

    if (M_IsDebug) { 
      Serial.println("\nConnected to: ");
      Serial.print(M_STA_SSID);
      Serial.println("\nIP address: ");
      Serial.println(WiFi.localIP());

      Serial.println("While Broadcasting network from: ");
      Serial.println(WiFi.softAPIP());
      Serial.println(WiFi.softAPSSID());
    }
  }

}