#include <unordered_map>

#include <ESPAsyncWebServer.h>
#include <WiFi.h>

#include "WebPages.hpp"
#include "Settings.hpp"

namespace Web {
    class WebSystemCaptiveRequestHandler : public AsyncWebHandler {
        public:
            WebSystemCaptiveRequestHandler() {}
            virtual ~WebSystemCaptiveRequestHandler() {}

            bool canHandle(AsyncWebServerRequest *request) const override {
                return true; 
            }

            void handleRequest(AsyncWebServerRequest *request) override {
                String AP_IP = WiFi.softAPIP().toString();
                
                if (request->host() == AP_IP || request->host() == "localhost") {
                    request->send(404, "text/plain", "Page not found");
                } else {
                    AsyncWebServerResponse *Response = request->beginResponse(302, "text/plain", "");
                    Response->addHeader("Location", "http://" + AP_IP + "/");
                    Response->addHeader("Connection", "close");
                    request->send(Response);
                }
            }
    };

    class WebSystem {
        public:
            struct Options{
                bool I_IsDebug = {false};

                Set::SettingsManager* I_DeviceSettings;
            };

            struct ArduinoStringHash {
                std::size_t operator()(const String& F_String) const {
                    return std::hash<std::string>{}(F_String.c_str());
                }
            };

        public:
            WebSystem(const Options& Init) : M_IsDebug(Init.I_IsDebug), M_IsFinished(false), M_DeviceSettings(Init.I_DeviceSettings) {}

            void SetupSystem(AsyncWebServer* F_WebServer);

        private:
            void HandleRoot(AsyncWebServerRequest* F_Request) {
                F_Request->send(200, "text/html", Web::AP_HTML_HOME);
            }
            void HandleNotFound(AsyncWebServerRequest* F_Request) {
                if (M_IsFinished) {
                    F_Request->send(200, "text/html", "<HTML><HEAD><TITLE>Success</TITLE></HEAD><BODY>Success</BODY></HTML>");
                }
                if (M_IsDebug) Serial.println("\nWeb-> Page not found");
                F_Request->redirect(("http://" + WiFi.softAPIP().toString() + "/").c_str());
            }

            void HandleGet_home(AsyncWebServerRequest* F_Request) {
                F_Request->send(200, "text/html", Web::AP_HTML_HOME);
            }
            void HandleGet_ns(AsyncWebServerRequest* F_Request) {
                F_Request->send(200, "text/html", Web::AP_HTML_NS_OPTIONS);
            }
            void HandleGet_wifi(AsyncWebServerRequest* F_Request) {
                F_Request->send(200, "text/html", Web::AP_HTML_WIFI_OPTIONS);
            }

            void HandlePost_reset(AsyncWebServerRequest* F_Request) {
                if (F_Request->hasParam("action", true)) {
                    M_IsFinished = true;
                    if (M_IsDebug) {
                        Serial.print("\n");
                        for (const auto& [InputName, InputData] : M_AP_Input) {
                            Serial.println("Web-> " + InputName + ": " + InputData);
                        }
                    }
                }

                auto InputValue = M_AP_Input.find("data-type");
                String* DataType = (InputValue != M_AP_Input.end()) ? &InputValue->second : nullptr;
                InputValue = M_AP_Input.find("ns-site");
                String* NS_SITE = (InputValue != M_AP_Input.end()) ? &InputValue->second : nullptr;
                InputValue = M_AP_Input.find("ns-secret");
                String* NS_SECRET = (InputValue != M_AP_Input.end()) ? &InputValue->second : nullptr;

                InputValue = M_AP_Input.find("sta-ssid");
                String* STA_SSID = (InputValue != M_AP_Input.end()) ? &InputValue->second : nullptr;
                InputValue = M_AP_Input.find("sta-password");
                String* STA_PASSWORD = (InputValue != M_AP_Input.end()) ? &InputValue->second : nullptr;

                InputValue = M_AP_Input.find("ap-ssid");
                String* AP_SSID = (InputValue != M_AP_Input.end()) ? &InputValue->second : nullptr;
                InputValue = M_AP_Input.find("ap-password");
                String* AP_PASSWORD = (InputValue != M_AP_Input.end()) ? &InputValue->second : nullptr;

                M_DeviceSettings->QueueReset(
                    Set::SettingsManager::OptionalSettings{
                        DataType,
                        NS_SITE,
                        NS_SECRET,

                        STA_SSID,
                        STA_PASSWORD,
                        
                        AP_SSID,
                        AP_PASSWORD
                    }
                );

                F_Request->send(200, "text/html", "<HTML><HEAD><TITLE>Success</TITLE></HEAD><BODY>Success</BODY></HTML>");
            }
            void HandlePost_ns(AsyncWebServerRequest* F_Request) {
                if (F_Request->hasParam("data-type", true)) {
                    M_AP_Input["data-type"] = F_Request->getParam("data-type", true)->value();
                }
                if (F_Request->hasParam("ns-site", true)) {
                    M_AP_Input["ns-site"] = F_Request->getParam("ns-site", true)->value();
                }
                if (F_Request->hasParam("ns-secret", true)) {
                    M_AP_Input["ns-secret"] = F_Request->getParam("ns-secret", true)->value();
                }

                F_Request->redirect(("http://" + WiFi.softAPIP().toString() + "/ns").c_str());
            }
            void HandlePost_wifi(AsyncWebServerRequest* F_Request) {
                if (F_Request->hasParam("sta-ssid", true)) {
                    M_AP_Input["sta-ssid"] = F_Request->getParam("sta-ssid", true)->value();
                }
                if (F_Request->hasParam("sta-password", true)) {
                    M_AP_Input["sta-password"] = F_Request->getParam("sta-password", true)->value();
                }
                if (F_Request->hasParam("ap-ssid", true)) {
                    M_AP_Input["ap-ssid"] = F_Request->getParam("ap-ssid", true)->value();
                }
                if (F_Request->hasParam("ap-password", true)) {
                    M_AP_Input["ap-password"] = F_Request->getParam("ap-password", true)->value();
                }

                F_Request->redirect(("http://" + WiFi.softAPIP().toString() + "/wifi").c_str());
            }

            void HandleAsset_css(AsyncWebServerRequest* F_Request) {
                F_Request->send(200, "text/css", Web::AP_CSS);
            }
            void HandleAsset_js(AsyncWebServerRequest* F_Request) {
                F_Request->send(200, "application/javascript", Web::AP_JS);
            }

        private:
            bool M_IsDebug = {false};
            bool M_IsFinished = {false};

            Set::SettingsManager* M_DeviceSettings;

            std::unordered_map<String, String, ArduinoStringHash> M_AP_Input;
    };
}