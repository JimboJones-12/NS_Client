#include "GUI.hpp"

namespace GUI {
  class Title : public GUIComponent {
    public:
      struct Options {
        PixelPartition I_PartitionVertical = {0, 0};
        PixelPartition I_PartitionHorizontal = {0, 0};

        String I_TitleString = {"EMPTY"};
      };

    public:
      Title(const Options& Init) : GUIComponent({Init.I_PartitionVertical, Init.I_PartitionHorizontal}) {
      }


      Title(Title&&) = default;
			Title& operator=(Title&&) = default;

			Title(const Title&) = default;
			Title& operator=(const Title&) = default;

      void RenderComponent(U8G2_OLED* F_OLED) override final {
        Serial.println("drawn compnt");
        F_OLED->drawStr(0, 0, "Hello World");
      }
      void TickComponent(U8G2_OLED* F_OLED) override final {
      }

    private:
      String M_PayLoadString = {"EMPTY"};
  };
}