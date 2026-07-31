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

  void NetworkSystem::Initialize(Web::WebSystem& F_WebSystem) {
    if (M_IsDebug) {
      Serial.printf("\nWiFi-> Connecting to STA network: \"%s\"\n", M_STA_SSID);
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

    unsigned long int WiFiLastTime = 0;
    unsigned long int WiFiConnectionAttempts = 0;

    if (M_IsDebug) Serial.print("WiFi-> ");
    while ((WiFi.softAPIP() == IPAddress(0, 0, 0, 0) || WiFi.status() != WL_CONNECTED) && WiFiConnectionAttempts <= 10) {
      esp_task_wdt_reset();
      delay(1);
      if (millis() - WiFiLastTime >= 1000) {
        if (M_IsDebug) Serial.print(". ");
        WiFiLastTime = millis();
        WiFiConnectionAttempts++;
      }
    }
    delay(2000);

    M_DNSServer.setErrorReplyCode(DNSReplyCode::NoError);
    M_DNSServer.start(M_DNS_PORT, "*", WiFi.softAPIP());

    if (M_IsDebug) {
      if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("\nWiFi-> Connected to STA network: \"%s\"\n", M_STA_SSID);
        Serial.printf("WiFi-> With IP address: \"%s\"\n", WiFi.localIP().toString());
      } else Serial.println("\nWiFi-> STA failed to connect"); 

      Serial.printf("WiFi-> Broadcasting AP network from: \"%s\"\n", WiFi.softAPIP().toString());
      Serial.printf("WiFi-> With SSID: \"%s\"\n", WiFi.softAPSSID());
    }
  }
}