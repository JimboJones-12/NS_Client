#include <WiFi.h>
#include <NetworkClientSecure.h>
#include <HTTPClient.h>

#include <WebServer.h>

#include "esp32_cert_bundle.h"

namespace Net {
  extern NetworkClientSecure NetworkClient;
  extern HTTPClient HTTPSClient;
  extern WebServer Server;

  extern char WiFi_SSID[];
  extern char WiFi_PASSWORD[];
  extern const String API_SECRET;
  extern const String API_REQUEST;

  extern char AP_SSID[];
  extern char AP_PASSWORD[];

  extern IPAddress AP_LOCAL_IP;
  extern IPAddress AP_GATEWAY;
  extern IPAddress AP_SUBNET;

  extern const char AP_HTML_PAGE[] PROGMEM;

  void Initialize();



  class NetworkSystem {
    public:
      struct Options {
        bool I_IsDebug = {false};

        int I_PORT;

        IPAddress I_AP_LOCAL_IP;
        IPAddress I_AP_GATEWAY;
        IPAddress I_AP_SUBNET;

        char I_AP_SSID[33];
        char I_AP_PASSWORD[65];
      };

    public:
      NetworkSystem(const Options& Init) : M_IsDebug(Init.I_IsDebug), M_Server(Init.I_PORT), M_AP_LOCAL_IP(Init.I_AP_LOCAL_IP), M_AP_GATEWAY(Init.I_AP_GATEWAY), M_AP_SUBNET(Init.I_AP_SUBNET) {
        CopyString(M_AP_SSID, Init.I_AP_SSID);
        CopyString(M_AP_PASSWORD, Init.I_AP_PASSWORD);
      }

      void WiFiInitialize() {
        if (M_IsDebug) {
          Serial.println("\nConnecting to network: ");
          Serial.print(M_WiFi_SSID);
          Serial.println("\n\n");
        }

        WiFi.mode(WIFI_AP_STA);

        WiFi.softAPConfig(M_AP_LOCAL_IP, M_AP_GATEWAY, M_AP_SUBNET);
        WiFi.softAP(M_AP_SSID, M_AP_PASSWORD);

        WiFi.begin(String(M_WiFi_SSID), String(M_WiFi_PASSWORD));

        M_Server.on("/", [&]() {
            M_Server.send_P(200, "text/html", AP_HTML_PAGE);
          }
        );
        M_Server.begin();

        if (M_IsDebug) { 
          Serial.println("\nConnected to: ");
          Serial.print(M_WiFi_SSID);
          Serial.println("\nIP address: ");
          Serial.println(WiFi.localIP());

          Serial.println("While Broadcasting network from: ");
          Serial.println(WiFi.softAPIP());
          Serial.println(WiFi.softAPSSID());
        }
      }

    private:
      char* CopyString(char* F_Destination, const char* F_Source) {
        char* Start = F_Destination;

        while (*F_Source != '\0') {
            *F_Destination = *F_Source;
            F_Destination ++;
            F_Source ++;
        }

        *F_Destination = '\0';
        return Start;
      }

    private:
      bool M_IsDebug = {false};

      NetworkClientSecure M_NetworkClient;
      HTTPClient M_HTTPSClient;
      WebServer M_Server;

      char M_WiFi_SSID[33];
      char M_WiFi_PASSWORD[65];

      char M_AP_SSID[33];
      char M_AP_PASSWORD[65];

      String M_API_SITE;
      String M_API_SECRET;
      String M_API_REQUEST;

      IPAddress M_AP_LOCAL_IP;
      IPAddress M_AP_GATEWAY;
      IPAddress M_AP_SUBNET;
  };
}