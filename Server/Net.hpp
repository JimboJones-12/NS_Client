#include <sys/_stdint.h>
#include <cstring>

#include <esp_wifi.h>
#include <esp_task_wdt.h>
#include <NetworkClientSecure.h>
#include <HTTPClient.h>
#include <DNSServer.h>

#include "Web.hpp"
#include "esp32_cert_bundle.h"

namespace Net {
  class NetworkSystem {
    public:
      struct Options {
        bool I_IsDebug = {false};
        bool I_STA_IsInsecure = {false};

        int I_HTTP_PORT = {80};
        int I_DNS_PORT = {53};

        Set::SettingsManager* I_DeviceSettings;

        IPAddress I_AP_LOCAL_IP = IPAddress(192,168,10,1);
        IPAddress I_AP_GATEWAY = IPAddress(192,168,10,1);
        IPAddress I_AP_SUBNET = IPAddress(255,255,255,0);

        int I_AP_CHANNEL;
        int I_AP_NETWORK_NAME_IS_HIDDEN = {0};
        int I_AP_MAX_DEVICES;

        const unsigned char* I_Certificates;
        size_t I_CertificateSize;
      };

      struct Packet {
        String Payload = {""};
        int Code = {404};
      };

    public:
      NetworkSystem(const Options& Init) : M_IsDebug(Init.I_IsDebug), M_HTTP_PORT(Init.I_HTTP_PORT), M_DNS_PORT(Init.I_DNS_PORT), M_WebServer(Init.I_HTTP_PORT), M_DeviceSettings(Init.I_DeviceSettings), M_AP_LOCAL_IP(Init.I_AP_LOCAL_IP), M_AP_GATEWAY_IP(Init.I_AP_GATEWAY), M_AP_SUBNET_IP(Init.I_AP_SUBNET), M_AP_CHANNEL(Init.I_AP_CHANNEL), M_AP_NETWORK_NAME_IS_HIDDEN(Init.I_AP_NETWORK_NAME_IS_HIDDEN), M_AP_MAX_DEVICES(Init.I_AP_MAX_DEVICES), M_Certificates(Init.I_Certificates), M_CertificateSize(Init.I_CertificateSize) {
        if (Init.I_STA_IsInsecure == false) {
          M_NetworkClient.setCACertBundle(M_Certificates, sizeof(*M_Certificates));
        } else {
          M_NetworkClient.setInsecure();
        }
      }

      void STA_Setup(const char* F_STA_SSID_NEW = nullptr, const char* F_STA_PASSWORD_NEW = nullptr) {
        if (F_STA_SSID_NEW != nullptr) {
          std::strncpy(M_STA_SSID, F_STA_SSID_NEW, 32);
          M_STA_SSID[32] = '\0';
        }
        if (F_STA_PASSWORD_NEW != nullptr) {
          std::strncpy(M_STA_PASSWORD, F_STA_PASSWORD_NEW, 64);
          M_STA_PASSWORD[64] = '\0';
        }
      }

      void AP_Setup(const char* F_AP_SSID_NEW = nullptr, const char* F_AP_PASSWORD_NEW = nullptr, IPAddress* F_AP_LOCAL_IP_NEW = nullptr, IPAddress* F_AP_GATEWAY_IP_NEW = nullptr, IPAddress* F_AP_SUBNET_IP_NEW = nullptr) {
        if (F_AP_SSID_NEW != nullptr) {
          std::strncpy(M_AP_SSID, F_AP_SSID_NEW, 32);
          M_AP_SSID[32] = '\0';
        }
        if (F_AP_PASSWORD_NEW != nullptr) {
          std::strncpy(M_AP_PASSWORD, F_AP_PASSWORD_NEW, 32);
          M_AP_PASSWORD[32] = '\0';
        }

        if (F_AP_LOCAL_IP_NEW != nullptr) {
          M_AP_LOCAL_IP = *F_AP_LOCAL_IP_NEW;
        }
        if (F_AP_GATEWAY_IP_NEW != nullptr) {
          M_AP_GATEWAY_IP = *F_AP_GATEWAY_IP_NEW;
        }
        if (F_AP_SUBNET_IP_NEW != nullptr) {
          M_AP_SUBNET_IP = *F_AP_SUBNET_IP_NEW;
        }
      }

      void NS_SITE_Setup(const String* F_API_SITE_NEW = nullptr, const String* F_API_SECRET_NEW = nullptr) {
        if (F_API_SITE_NEW != nullptr) {
          M_NS_SITE = *F_API_SITE_NEW;
        }
        if (F_API_SECRET_NEW != nullptr) {
          M_NS_SECRET = *F_API_SECRET_NEW;
        }

        M_NS_REQUEST = M_NS_SITE + "api/v1/entries.json?count=1&api_secret=" + M_NS_SECRET;
      }

      void Initialize(Web::WebSystem& F_WebSystem);

      void STA_Tick() {
        if (M_HTTPClient.begin(M_NetworkClient, M_NS_REQUEST)) {
          if (M_IsDebug) Serial.print("\nHTTP-> Sent request to server.");

          M_HTTP_LastCode = M_HTTPClient.GET();
          if (M_HTTP_LastCode > 0) {
            M_HTTP_LastPayload = M_HTTPClient.getString();

            if (M_IsDebug) {
              Serial.printf("\nHTTP-> GET code: \"%d\"\n", M_HTTP_LastCode);
              Serial.printf("HTTP-> GET payload: \"%s\"\n", M_HTTP_LastPayload);
            }
          } 
          else if (M_IsDebug) Serial.printf("\nHTTP-> GET failed. Error: \"%s\"\n", M_HTTPClient.errorToString(M_HTTP_LastCode).c_str());
        }
        M_HTTPClient.end();
      }

      void AP_Tick(unsigned int F_DNSTime = 100) {
        if (WiFi.getMode() & WIFI_AP_STA) {
          static unsigned int DNSLastTime = millis();

          if (millis() - DNSLastTime > F_DNSTime) M_DNSServer.processNextRequest();
        }
      }

      Packet GetPacket() const {
        return {M_HTTP_LastPayload, M_HTTP_LastCode};
      }

    private:


    private:
      bool M_IsDebug = {false};

      int M_HTTP_PORT = {80};
      int M_DNS_PORT = {53};

      NetworkClientSecure M_NetworkClient;
      HTTPClient M_HTTPClient;
      AsyncWebServer M_WebServer;
      DNSServer M_DNSServer;

      Set::SettingsManager* M_DeviceSettings;

      const unsigned char* M_Certificates = {nullptr};
      size_t M_CertificateSize;

      char M_STA_SSID[33];
      char M_STA_PASSWORD[65];

      char M_AP_SSID[33];
      char M_AP_PASSWORD[65];

      int M_AP_CHANNEL;
      int M_AP_NETWORK_NAME_IS_HIDDEN = {0};
      int M_AP_MAX_DEVICES;

      String M_NS_SITE;
      String M_NS_SECRET;
      String M_NS_REQUEST;

      IPAddress M_AP_LOCAL_IP;
      IPAddress M_AP_GATEWAY_IP;
      IPAddress M_AP_SUBNET_IP;

      String M_HTTP_LastPayload;
      int M_HTTP_LastCode;
  };
}