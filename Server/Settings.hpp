#include <Preferences.h>
#include <Arduino.h>

namespace Set {
    class SettingsManager {
        public:
            struct Settings {
                String M_DataType = {"mmol/L"};
                String M_NS_SITE = {""};
                String M_NS_SECRET = {""};

                String M_STA_SSID = {""};
                String M_STA_PASSWORD = {""};

                String M_AP_SSID = {"Glucose Monitor"};
                String M_AP_PASSWORD = {"12345678"};
            };
            struct OptionalSettings {
                String* M_DataType = {nullptr};
                String* M_NS_SITE = {nullptr};
                String* M_NS_SECRET = {nullptr};

                String* M_STA_SSID = {nullptr};
                String* M_STA_PASSWORD = {nullptr};

                String* M_AP_SSID = {nullptr};
                String* M_AP_PASSWORD = {nullptr};
            };

            struct Options {
                bool I_IsDebug = {false};
            };
            
        public:
            SettingsManager(const Options& Init) : M_IsDebug(Init.I_IsDebug), M_NeedReset(false) {}

            void StartupCheck() {
                M_Preferences.begin("settings", true);

                if (!(M_Preferences.isKey("config"))) {
                    M_Preferences.end();
                    M_Preferences.begin("settings", false);

                    M_Preferences.putBool("config", true);

                    M_Preferences.putString("data-type", M_DefaultSettings.M_DataType);
                    M_Preferences.putString("ns-site", M_DefaultSettings.M_NS_SITE);
                    M_Preferences.putString("ns-secret", M_DefaultSettings.M_NS_SECRET);

                    M_Preferences.putString("sta-ssid", M_DefaultSettings.M_STA_SSID);
                    M_Preferences.putString("sta-password", M_DefaultSettings.M_STA_PASSWORD);

                    M_Preferences.putString("ap-ssid", M_DefaultSettings.M_AP_SSID);
                    M_Preferences.putString("ap-password", M_DefaultSettings.M_AP_PASSWORD);

                    M_Preferences.end();
                    M_Preferences.begin("settings", true);
                }

                M_CurrentSettings.M_DataType = M_Preferences.getString("data-type");
                M_CurrentSettings.M_NS_SITE = M_Preferences.getString("ns-site");
                M_CurrentSettings.M_NS_SECRET = M_Preferences.getString("ns-secret");

                M_CurrentSettings.M_STA_SSID = M_Preferences.getString("sta-ssid");
                M_CurrentSettings.M_STA_PASSWORD = M_Preferences.getString("sta-password");
                
                M_CurrentSettings.M_AP_SSID = M_Preferences.getString("ap-ssid");
                M_CurrentSettings.M_AP_PASSWORD = M_Preferences.getString("ap-password");

                M_Preferences.end();

                if (M_IsDebug) {
                    Serial.println("\nSettings-> DataType: " + M_CurrentSettings.M_DataType);
                    Serial.println("Settings-> NS_SITE: " + M_CurrentSettings.M_NS_SITE);
                    Serial.println("Settings-> NS_SECRET: " + M_CurrentSettings.M_NS_SECRET);

                    Serial.println("Settings-> STA_SSID: " + M_CurrentSettings.M_STA_SSID);
                    Serial.println("Settings-> STA_PASSWORD: " + M_CurrentSettings.M_STA_PASSWORD);

                    Serial.println("Settings-> AP_SSID: " + M_CurrentSettings.M_AP_SSID);
                    Serial.println("Settings-> AP_PASSWORD: " + M_CurrentSettings.M_AP_PASSWORD);
                }
            }

            void QueueReset(const OptionalSettings& F_NewSettings) {
                M_QueuedSettings = F_NewSettings;
                M_NeedReset = true;
            }

            void SetSettings(const OptionalSettings& F_NewSettings) {
                Settings TemporarySettings = {
                    (F_NewSettings.M_DataType != nullptr) ? *(F_NewSettings.M_DataType) : M_CurrentSettings.M_DataType,
                    (F_NewSettings.M_NS_SITE != nullptr) ? *(F_NewSettings.M_NS_SITE) : M_CurrentSettings.M_NS_SITE,
                    (F_NewSettings.M_NS_SECRET != nullptr) ? *(F_NewSettings.M_NS_SECRET) : M_CurrentSettings.M_NS_SECRET,

                    (F_NewSettings.M_STA_SSID != nullptr) ? *(F_NewSettings.M_STA_SSID) : M_CurrentSettings.M_STA_SSID,
                    (F_NewSettings.M_STA_PASSWORD != nullptr) ? *(F_NewSettings.M_STA_PASSWORD) : M_CurrentSettings.M_STA_PASSWORD,
                    
                    (F_NewSettings.M_AP_SSID != nullptr) ? *(F_NewSettings.M_AP_SSID) : M_CurrentSettings.M_AP_SSID,
                    (F_NewSettings.M_AP_PASSWORD != nullptr) ? *(F_NewSettings.M_AP_PASSWORD) : M_CurrentSettings.M_AP_PASSWORD
                };
                M_CurrentSettings = TemporarySettings;

                M_Preferences.begin("settings", false);

                M_Preferences.putString("data-type", M_CurrentSettings.M_DataType);
                M_Preferences.putString("ns-site", M_CurrentSettings.M_NS_SITE);
                M_Preferences.putString("ns-secret", M_CurrentSettings.M_NS_SECRET);

                M_Preferences.putString("sta-ssid", M_CurrentSettings.M_STA_SSID);
                M_Preferences.putString("sta-password", M_CurrentSettings.M_STA_PASSWORD);

                M_Preferences.putString("ap-ssid", M_CurrentSettings.M_AP_SSID);
                M_Preferences.putString("ap-password", M_CurrentSettings.M_AP_PASSWORD);

                M_Preferences.end();
            }

            void CheckReset() {
                if (M_NeedReset) {
                    M_NeedReset = false;
                    SetSettings(M_QueuedSettings);
                } else {
                    return;
                }
                Serial.flush();
                ESP.restart();
            }

            Settings GetSettings() const {
                return M_CurrentSettings;
            }

        private:

        private:
            Settings M_DefaultSettings;
            Settings M_CurrentSettings;
            OptionalSettings M_QueuedSettings;

            Preferences M_Preferences;
            
            bool M_IsDebug = {false};
            bool M_IsConfigured = {false};
            bool M_NeedReset = {false};

    };
}