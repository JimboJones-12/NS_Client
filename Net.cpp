#include "Net.hpp"

namespace Net {
  NetworkClientSecure NetworkClient;
  HTTPClient HTTPSClient;
  WebServer Server(80);

  char WiFi_SSID[] = "Cleb_Club";
  char WiFi_PASSWORD[] = "clebclubwifi";

  const String API_SITE = "https://37741.cgm.bcdiabetes.ca/";
  const String API_SECRET = "XIO-52361047";
  const String API_REQUEST = API_SITE + "api/v1/entries.json?count=1&api_secret=" + API_SECRET;

  char AP_SSID[] = "ESP32_AP";
  char AP_PASSWORD[] = "espPassword";

  IPAddress AP_LOCAL_IP(192,168,10,1);
  IPAddress AP_GATEWAY(192,168,10,1);
  IPAddress AP_SUBNET(255,255,255,0);

  const char AP_HTML_PAGE[] PROGMEM = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <title>ESP32 Control</title>
    </head>
    <body>
      <h1>ESP32 Dashboard</h1>
      <button onclick="fetch('/led/on')">LED ON</button>
    </body>
    </html>
  )rawliteral";

  void Initialize() {
    Serial.println("\nConnecting to network: ");
    Serial.print(Net::WiFi_SSID);
    Serial.println("\n\n");

    WiFi.mode(WIFI_AP_STA);

    WiFi.softAPConfig(
        IPAddress(192,168,10,1),
        IPAddress(192,168,10,1),
        IPAddress(255,255,255,0)
    );
    WiFi.softAP("ESP32_AP", "12345678");

    WiFi.begin(Net::WiFi_SSID, Net::WiFi_PASSWORD);
    /*unsigned int Counter = 0;
    while (WiFi.status() != WL_CONNECTED) {
      if (Counter > 10) {
        Counter = 0;
        Serial.println(". ");
      } else Serial.print(". ");

      Counter ++;
      delay(500);
    }*/

    Serial.println("\nConnected to: ");
    Serial.print(Net::WiFi_SSID);
    Serial.println("\nIP address: ");
    Serial.println(WiFi.localIP());

    Serial.println("While Broadcasting network from: ");
    Serial.println(WiFi.softAPIP());
    Serial.println(WiFi.softAPSSID());

    Net::Server.on("/", []() {
      Net::Server.send_P(200, "text/html", Net::AP_HTML_PAGE);
    });
    Net::Server.begin();
  }
}