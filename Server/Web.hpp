#include <string>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <Arduino.h>

#include "WebPages.hpp"

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
            };

        public:
            WebSystem(const Options& Init) : M_IsDebug(Init.I_IsDebug) {}

            void SetupSystem(AsyncWebServer* F_WebServer, IPAddress& F_AP_LOCAL_IP) {
                DefaultHeaders::Instance().addHeader("Connection", "close");

                F_WebServer->on(
                    "/",
                    HTTP_GET,
                    [this] (AsyncWebServerRequest* F_Request) {
                        this->HandleRoot(F_Request);
                    }
                );
                F_WebServer->onNotFound(
                    [this, &F_AP_LOCAL_IP] (AsyncWebServerRequest* F_Request) {
                        this->HandleNotFound(F_Request, F_AP_LOCAL_IP);
                    }
                );


                F_WebServer->on(
                    "/home.html",
                    HTTP_GET,
                    [this] (AsyncWebServerRequest* F_Request) {
                        this->HandleGet_home(F_Request);
                    }
                );
                F_WebServer->on(
                    "/ns.html",
                    HTTP_GET,
                    [this] (AsyncWebServerRequest* F_Request) {
                        this->HandleGet_ns(F_Request);
                    }
                );
                F_WebServer->on(
                    "/wifi.html",
                    HTTP_GET,
                    [this] (AsyncWebServerRequest* F_Request) {
                        this->HandleGet_wifi(F_Request);
                    }
                );

                F_WebServer->on(
                    "/styles.css",
                    [this] (AsyncWebServerRequest* F_Request) {
                        this->HandleAsset_css(F_Request);
                    }
                );
                F_WebServer->on(
                    "/app.js",
                    [this] (AsyncWebServerRequest* F_Request) {
                        this->HandleAsset_js(F_Request);
                    }
                );

                F_WebServer->on(
                    "/api/reset",
                    HTTP_POST,
                    [this] (AsyncWebServerRequest* F_Request) {
                        this->HandlePost_reset(F_Request);
                    }
                );
                F_WebServer->on(
                    "/api/ns-submission",
                    HTTP_POST,
                    [this] (AsyncWebServerRequest* F_Request) {
                        this->HandlePost_ns(F_Request);
                    }
                );
                F_WebServer->on(
                    "/api/wifi-submission",
                    HTTP_POST,
                    [this] (AsyncWebServerRequest* F_Request) {
                        this->HandlePost_wifi(F_Request);
                    }
                );

                F_WebServer->addHandler(new WebSystemCaptiveRequestHandler()).setFilter(ON_AP_FILTER);
            }

        private:
            void HandleRoot(AsyncWebServerRequest* F_Request) {
                F_Request->send(200, "text/html", Web::AP_HTML_HOME);
            }
            void HandleNotFound(AsyncWebServerRequest* F_Request, IPAddress& F_AP_LOCAL_IP) {
                Serial.println(F_Request->url());

                if (F_Request->url() == "/" || F_Request->url() == "/home.html" || F_Request->url() == "/home") {
                    F_Request->send(200, "text/html", Web::AP_HTML_HOME);
                } else if (F_Request->url() == "/ns.html" || F_Request->url() == "/ns") {
                    F_Request->send(200, "text/html", Web::AP_HTML_NS_OPTIONS);
                } else if (F_Request->url() == "/wifi.html" || F_Request->url() == "/wifi") {
                    F_Request->send(200, "text/html", Web::AP_HTML_WIFI_OPTIONS);
                } else if (F_Request->url() == "/api/reset") {
                    String Data = "";
                    if (F_Request->hasParam("action", true)) {
                        Data = F_Request->getParam("action", true)->value();
                    }
                    Serial.println(Data);
                    F_Request->send(200, "text/html", Web::AP_HTML_HOME);
                } else if (F_Request->url() == "/api/ns-submission") {
                    F_Request->send(200, "text/html", Web::AP_HTML_HOME);
                } else if (F_Request->url() == "/api/wifi-submission") {
                    F_Request->send(200, "text/html", Web::AP_HTML_HOME);
                } else {
                    Serial.println("Page not found");
                    F_Request->redirect(("http://" + F_AP_LOCAL_IP.toString() + "/home.html").c_str());
                }
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
                String Data = "";
                if (F_Request->hasParam("action", true)) {
                    Data = F_Request->getParam("action", true)->value();
                }
                Serial.println("\n\nData\n\n");

                F_Request->redirect("/");
            }
            void HandlePost_ns(AsyncWebServerRequest* F_Request) {
                String Data[3];
                if (F_Request->hasParam("data-type", true)) {
                    Data[0] = F_Request->getParam("data-type", true)->value();
                }
                if (F_Request->hasParam("ns-site", true)) {
                    Data[1] = F_Request->getParam("ns-site", true)->value();
                }
                if (F_Request->hasParam("ns-secret", true)) {
                    Data[2] = F_Request->getParam("ns-secret", true)->value();
                }
                Serial.print("\n");
                for(auto& obj : Data) {
                    Serial.println(obj);
                }
                Serial.print("\n");

                F_Request->redirect("/");
            }
            void HandlePost_wifi(AsyncWebServerRequest* F_Request) {
                String Data[4];
                if (F_Request->hasParam("sta-ssid", true)) {
                    Data[0] = F_Request->getParam("sta-ssid", true)->value();
                }
                if (F_Request->hasParam("sta-password", true)) {
                    Data[1] = F_Request->getParam("sta-password", true)->value();
                }
                if (F_Request->hasParam("ap-ssid", true)) {
                    Data[2] = F_Request->getParam("ap-ssid", true)->value();
                }
                if (F_Request->hasParam("ap-password", true)) {
                    Data[3] = F_Request->getParam("ap-password", true)->value();
                }
                Serial.print("\n");
                for(auto& obj : Data) {
                    Serial.println(obj);
                }
                Serial.print("\n");

                F_Request->redirect("/");
            }

            void HandleAsset_css(AsyncWebServerRequest* F_Request) {
                F_Request->send(200, "text/css", Web::AP_CSS);
            }
            void HandleAsset_js(AsyncWebServerRequest* F_Request) {
                F_Request->send(200, "application/javascript", Web::AP_JS);
            }

        private:
            bool M_IsDebug = {false};
    };
}