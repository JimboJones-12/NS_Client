#include "Net.hpp"

namespace Net {

  //char WiFi_SSID[] = "Cleb_Club";
  //char WiFi_PASSWORD[] = "clebclubwifi";

  //const String API_SITE = "https://37741.cgm.bcdiabetes.ca/";
  //const String API_SECRET = "XIO-52361047";
  //const String API_REQUEST = API_SITE + "api/v1/entries.json?count=1&api_secret=" + API_SECRET;

  //char AP_SSID[] = "ESP32_AP";
  //char AP_PASSWORD[] = "espPassword";

  const char AP_HTML_PAGE[] PROGMEM = {
    R"rawliteral(
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
    )rawliteral"
  };

}