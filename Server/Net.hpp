#include <sys/_stdint.h>
#include <cstring>

#include <esp_wifi.h>
#include <WiFi.h>
#include <NetworkClientSecure.h>
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>

#include <WebPages.hpp>
#include "esp32_cert_bundle.h"

namespace Net {

  extern const char AP_HTML_PAGE[] PROGMEM;

  class NetworkSystem {
    public:
      struct Options {
        bool I_IsDebug = {false};
        bool I_STA_IsInsecure = {false};

        int I_HTTP_PORT = {80};
        int I_DNS_PORT = {53};

        IPAddress I_AP_LOCAL_IP = IPAddress(192,168,10,1);
        IPAddress I_AP_GATEWAY = IPAddress(192,168,10,1);
        IPAddress I_AP_SUBNET = IPAddress(255,255,255,0);

        char I_STA_SSID[33];
        char I_STA_PASSWORD[65];

        char I_AP_SSID[33] = {"N.S. Monitor"};
        char I_AP_PASSWORD[65] = {"1234"};

        String I_API_SITE;
        String I_API_SECRET;

        const unsigned char* I_Certificates;
        size_t I_CertificateSize;
      };

      struct Packet {
        String Payload = {""};
        int Code = {404};
      };

    public:
      NetworkSystem(const Options& Init) : M_IsDebug(Init.I_IsDebug), M_HTTP_PORT(Init.I_HTTP_PORT), M_DNS_PORT(Init.I_DNS_PORT), M_WebServer(Init.I_HTTP_PORT), M_AP_LOCAL_IP(Init.I_AP_LOCAL_IP), M_AP_GATEWAY_IP(Init.I_AP_GATEWAY), M_AP_SUBNET_IP(Init.I_AP_SUBNET), M_API_SITE(Init.I_API_SITE), M_API_SECRET(Init.I_API_SECRET), M_Certificates(Init.I_Certificates), M_CertificateSize(Init.I_CertificateSize) {
        std::strncpy(M_AP_SSID, Init.I_AP_SSID, 32);
        M_AP_SSID[32] = '\0';
        std::strncpy(M_AP_PASSWORD, Init.I_AP_PASSWORD, 64);
        M_AP_PASSWORD[32] = '\0';

        std::strncpy(M_STA_SSID, Init.I_STA_SSID, 32);
        M_STA_SSID[32] = '\0';
        std::strncpy(M_STA_PASSWORD, Init.I_STA_PASSWORD, 64);
        M_STA_PASSWORD[32] = '\0';

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

      void NS_SITE_Setup(const String* F_API_SITE_NEW = nullptr, const String* F_API_SECRET_NEW = nullptr, IPAddress* F_AP_LOCAL_IP = nullptr, IPAddress* F_AP_GATEWAY = nullptr, IPAddress* F_AP_SUBNET = nullptr) {
        if (F_API_SITE_NEW != nullptr) {
          M_API_SITE = *F_API_SITE_NEW;
        }
        if (F_API_SECRET_NEW != nullptr) {
          M_API_SECRET = *F_API_SECRET_NEW;
        }

        M_API_REQUEST = M_API_SITE + "api/v1/entries.json?count=1&api_secret=" + M_API_SECRET;
      }

      void WiFiInitialize() {
        if (M_IsDebug) {
          Serial.println("\nConnecting to network: ");
          Serial.print(M_STA_SSID);
          Serial.println("\n\n");
        }

        WiFi.mode(WIFI_AP_STA);

        WiFi.softAPConfig(M_AP_LOCAL_IP, M_AP_GATEWAY_IP, M_AP_SUBNET_IP);
        WiFi.softAP(M_AP_SSID, M_AP_PASSWORD);

        WiFi.begin(String(M_STA_SSID), String(M_STA_PASSWORD));

        delay(500);

        M_WebServer.on("/", HTTP_GET, [&](AsyncWebServerRequest *F_Request) {this->WebServer_HandleNotFound(F_Request);});
        M_WebServer.on("/home.html", HTTP_GET, [&](AsyncWebServerRequest *F_Request) {this->WebServer_HandleNotFound(F_Request);});
        M_WebServer.on("/wifi.html", HTTP_GET, [&](AsyncWebServerRequest *F_Request) {this->WebServer_HandleNotFound(F_Request);});
        M_WebServer.on("/ns.html", HTTP_GET, [&](AsyncWebServerRequest *F_Request) {this->WebServer_HandleNotFound(F_Request);});

        M_WebServer.on("/styles.css", [&](AsyncWebServerRequest *F_Request) {this->WebServer_HandleCSS(F_Request);});
        M_WebServer.on("/app.js", [&](AsyncWebServerRequest *F_Request) {this->WebServer_HandleJS(F_Request);});

        M_WebServer.onNotFound([&](AsyncWebServerRequest *F_Request) {Serial.printf("404 Not Found - User tried to access: %s\n", F_Request->url().c_str());});
        M_WebServer.begin();

        while (WiFi.softAPIP() == IPAddress(0, 0, 0, 0)) {
          delay(100);
        }

        M_DNSServer.setErrorReplyCode(DNSReplyCode::NoError); // Prevents browser connection errors
        //M_DNSServer.start(M_DNS_PORT, "*", WiFi.softAPIP());

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

      void STA_Tick() {
        if (M_HTTPClient.begin(M_NetworkClient, M_API_REQUEST)) {
          if (M_IsDebug) Serial.println("Connected to server!");

          M_HTTP_LastCode = M_HTTPClient.GET();
          if (M_HTTP_LastCode > 0) {
            M_HTTP_LastPayload = M_HTTPClient.getString();

            if (M_IsDebug) {
              Serial.printf("[HTTPS] GET code: %d\n", M_HTTP_LastCode);
              Serial.printf("[HTTPS] GET payload: ");
            }
          } 
          else if (M_IsDebug) Serial.printf("[HTTPS] GET... failed, error: %s\n", M_HTTPClient.errorToString(M_HTTP_LastCode).c_str());
        }
        M_HTTPClient.end();
      }

      void AP_Tick(unsigned int F_DNSTime = 100, unsigned int F_WebTime = 100) {
        if (WiFi.getMode() & WIFI_AP_STA) {
          static unsigned int DNSLastTime = 0;
          static unsigned int WebLastTime = 0;

          if (millis() - DNSLastTime > F_DNSTime) M_DNSServer.processNextRequest();
        }

        delay(2);
      }

      Packet GetPacket() const {
        return {M_HTTP_LastPayload, M_HTTP_LastCode};
      }

    private:
      void WebServer_HandleNotFound(AsyncWebServerRequest *F_Request) {
        String uri = F_Request->url();
  
        if (uri == "/home") {
          WebServer_HandleHome(F_Request);
        } else if (uri == "/device") {
          WebServer_HandleDevice(F_Request);
        } else if (uri == "/ns") {
          WebServer_HandleNS(F_Request);
        } else {
          WebServer_HandleHome(F_Request);
        }
      }

      void WebServer_HandleHome(AsyncWebServerRequest *F_Request) {
        F_Request->send(200, "text/html", Web::AP_HTML_HOME);
      }
      void WebServer_HandleDevice(AsyncWebServerRequest *F_Request) {
        F_Request->send(200, "text/html", Web::AP_HTML_DEVICE_OPTIONS);
      }
      void WebServer_HandleNS(AsyncWebServerRequest *F_Request) {
        F_Request->send(200, "text/html", Web::AP_HTML_NS_OPTIONS);
      }

      void WebServer_HandleCSS(AsyncWebServerRequest *F_Request) {
        F_Request->send(200, "text/css", Web::AP_CSS);
      }
      void WebServer_HandleJS(AsyncWebServerRequest *F_Request) {
        F_Request->send(200, "application/javascript", Web::AP_JS);
      }

    private:
      bool M_IsDebug = {false};

      int M_HTTP_PORT = {80};
      int M_DNS_PORT = {53};

      NetworkClientSecure M_NetworkClient;
      HTTPClient M_HTTPClient;
      AsyncWebServer M_WebServer;
      DNSServer M_DNSServer;

      const unsigned char* M_Certificates = {nullptr};
      size_t M_CertificateSize;

      char M_STA_SSID[33];
      char M_STA_PASSWORD[65];

      char M_AP_SSID[33];
      char M_AP_PASSWORD[65];

      String M_API_SITE;
      String M_API_SECRET;
      String M_API_REQUEST;

      IPAddress M_AP_LOCAL_IP;
      IPAddress M_AP_GATEWAY_IP;
      IPAddress M_AP_SUBNET_IP;

      String M_HTTP_LastPayload;
      int M_HTTP_LastCode;
  };
}