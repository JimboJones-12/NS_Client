#include "Web.hpp"

namespace Web {
    void WebSystem::SetupSystem(AsyncWebServer* F_WebServer) {
                DefaultHeaders::Instance().addHeader("Connection", "close");

                F_WebServer->on(
                    "/",
                    HTTP_GET,
                    [this] (AsyncWebServerRequest* F_Request) {
                        this->HandleRoot(F_Request);
                    }
                );
                F_WebServer->onNotFound(
                    [this] (AsyncWebServerRequest* F_Request) {
                        this->HandleNotFound(F_Request);
                    }
                );

                F_WebServer->on(
                    "/home",
                    HTTP_GET,
                    [this] (AsyncWebServerRequest* F_Request) {
                        this->HandleGet_home(F_Request);
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
                    "/ns",
                    HTTP_GET,
                    [this] (AsyncWebServerRequest* F_Request) {
                        this->HandleGet_ns(F_Request);
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
                    "/wifi",
                    HTTP_GET,
                    [this] (AsyncWebServerRequest* F_Request) {
                        this->HandleGet_wifi(F_Request);
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
}