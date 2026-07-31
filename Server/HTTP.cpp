#include "HTTP.hpp"

namespace Web { 
    void HTTPSystem::HTTP_Tick(unsigned int F_HTTPTime = 100) {
        static unsigned int HTTPLastTime = millis();

        if (millis() - HTTPLastTime > F_HTTPTime) {
            M_NetworkSystem->STA_Tick();

            if (M_NetworkSystem->GetPacket().Code > 0) {
                JsonDocument DataDecoder;
                DeserializationError HasDecodeFailed = deserializeJson(DataDecoder, M_NetworkSystem->GetPacket().Payload);

                if (HasDecodeFailed) {
                    if (M_IsDebug) {
                        Serial.print("JSON deserialization failed: ");
                        Serial.println(HasDecodeFailed.c_str());
                    }
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
    }
}