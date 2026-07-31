#include <ArduinoJson.h>

#include "Net.hpp"

namespace Web {
    class HTTPSystem {
        public:
            struct Options {
                bool I_IsDebug = {false};

                Net::NetworkSystem* I_NetworkSystem;
            };

        public:
            HTTPSystem(const Options& Init) : M_IsDebug(Init.I_IsDebug), M_NetworkSystem(Init.I_NetworkSystem) {}

            void HTTP_Tick(unsigned int F_HTTPTime);

        private:

        private:
            bool M_IsDebug = {false};

            Net::NetworkSystem* M_NetworkSystem;
    };
}