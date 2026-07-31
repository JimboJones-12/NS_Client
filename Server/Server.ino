#include "GUI_Components.hpp"
#include "HTTP.hpp"

const int LOOP_TIME = 1000;
int LastTime = 0;

const bool IS_DEBUG = true;

enum class State {
  SETUP = 0,
  ACTIVE_IDLE = 1,
  PASSIVE_IDLE = 2,
  ACTIVE_MONITOR = 3,
  PASSIVE_MONITOR = 4
};
State AppState = State::SETUP;

Set::SettingsManager DeviceSettings(
  Set::SettingsManager::Options{
    .I_IsDebug = IS_DEBUG
  }
);

Net::NetworkSystem NetSys(
  Net::NetworkSystem::Options{
    .I_IsDebug = IS_DEBUG,
    .I_STA_IsInsecure = false,
    .I_HTTP_PORT = 80,
    .I_DNS_PORT = 53,

    .I_DeviceSettings = &DeviceSettings,

    .I_AP_LOCAL_IP = IPAddress(192,168,10,1),
    .I_AP_GATEWAY = IPAddress(192,168,10,1),
    .I_AP_SUBNET = IPAddress(255,255,255,0),

    .I_AP_CHANNEL = 0,
    .I_AP_NETWORK_NAME_IS_HIDDEN = 0,
    .I_AP_MAX_DEVICES = 4,

    .I_Certificates = x509_crt_bundle,
    .I_CertificateSize = x509_crt_bundle_len
  }
);

Web::WebSystem WebSys(
  Web::WebSystem::Options{
    .I_IsDebug = IS_DEBUG,
    .I_DeviceSettings = &DeviceSettings
  }
);

Web::HTTPSystem HTTPSys(
  Web::HTTPSystem::Options{
    .I_IsDebug = true,
    .I_NetworkSystem = &NetSys
  }
);

GUI::GUIInput InputSys;

GUI::GUIContainer GUISys(
  GUI::GUIContainer::Options{
    .I_PIN_SCL = 14,
    .I_PIN_SDA = 13,

    .I_GUIInput = &InputSys
  }
);

void setup() {
  Serial.begin(115200);

  esp_task_wdt_deinit();
  esp_task_wdt_config_t WDTConfig = {
    .timeout_ms = 10000,
    .idle_core_mask = (1 << portNUM_PROCESSORS) - 1,
    .trigger_panic = true
  };
  esp_task_wdt_init(&WDTConfig);
  esp_task_wdt_add(NULL);

  GUISys.AddPage(GUI::GUIPage::Options{
      .I_PageName = "Home"
    }
  );
  GUISys.GetPage("Home")->AddComponent(new GUI::Title(GUI::Title::Options{}));

  GUISys.Init();
  GUISys.RenderCurrentPage();
}

void loop() {
  esp_task_wdt_reset();
  delay(1);

  switch (AppState) {
    case State::SETUP: {
      DeviceSettings.StartupCheck();

      String NS_SITE = DeviceSettings.GetSettings().M_NS_SITE;
      String NS_SECRET = DeviceSettings.GetSettings().M_NS_SECRET;

      NetSys.AP_Setup(DeviceSettings.GetSettings().M_AP_SSID.c_str(), DeviceSettings.GetSettings().M_AP_PASSWORD.c_str(), nullptr, nullptr, nullptr);
      NetSys.STA_Setup(DeviceSettings.GetSettings().M_STA_SSID.c_str(), DeviceSettings.GetSettings().M_STA_PASSWORD.c_str());
      NetSys.NS_SITE_Setup(&NS_SITE, &NS_SECRET);

      NetSys.Initialize(WebSys);

      AppState = State::ACTIVE_IDLE;
    }
    break;

    case State::ACTIVE_IDLE: {
      NetSys.AP_Tick(1);
      
    }
    break;

    case State::PASSIVE_IDLE: {

    }
    break;

    case State::ACTIVE_MONITOR: {
      if (millis() - LastTime >= LOOP_TIME) {
        LastTime = millis();
      } else return;

      NetSys.STA_Tick();

      if (NetSys.GetPacket().Code > 0) {
        JsonDocument DataDecoder;
        DeserializationError HasDecodeFailed = deserializeJson(DataDecoder, NetSys.GetPacket().Payload);

        if (HasDecodeFailed) {
          Serial.print("JSON deserialization failed: ");
          Serial.println(HasDecodeFailed.c_str());
          return;
        }

        String DataType = DataDecoder[0]["type"];
        if (DataType == "sgv") {
          int SGV = DataDecoder[0]["sgv"];
          Serial.print(double(SGV) * (10000.0 / 180155.9));
          Serial.println("mmol/L");
        }
        if (DataType == "mbg") {
          int MBG = DataDecoder[0]["sgv"];
          Serial.println(double(MBG) * (10000.0 / 180155.9));
          Serial.println("mmol/L");
        }
      }
    }
    break;

    case State::PASSIVE_MONITOR: {

    }
    break;
  }

  DeviceSettings.CheckReset();
}