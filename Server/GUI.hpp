#include <U8g2lib.h>
#include <Arduino.h>

#include <vector>
#include <utility>

namespace GUI {
  class GUIInput {
    public:
      enum class State {
        INACTIVE,
        UP,
        DOWN,
        LEFT,
        RIGHT,
        SELECT,
        BACK
      };

      State ButtonState = State::INACTIVE;

    public:
      GUIInput() {}

      GUIInput(GUIInput&&) = default;
			GUIInput& operator=(GUIInput&&) = default;

			GUIInput(const GUIInput&) = delete;
			GUIInput& operator=(const GUIInput&) = delete;

      State GetState() const {
        return ButtonState;
      }

      void UpdateState() {
        unsigned int ActiveButtons = 0;
        State NewState = State::INACTIVE;

        if (1 == 1) {
          NewState = State::UP;
          ActiveButtons ++;
        }
        if (1 == 1) {
          NewState = State::DOWN;
          ActiveButtons ++;
        }
        if (1 == 1) {
          NewState = State::LEFT;
          ActiveButtons ++;
        }
        if (1 == 1) {
          NewState = State::RIGHT;
          ActiveButtons ++;
        }
        if (1 == 1) {
          NewState = State::SELECT;
          ActiveButtons ++;
        }
        if (1 == 1) {
          NewState = State::BACK;
          ActiveButtons ++;
        }

        if (ActiveButtons == 1) {
          ButtonState = NewState;
        }
      }

    private:
    
  };

  struct PixelPartition {
    unsigned int PartitionMin = {0};
    unsigned int PartitionMax = {0};
  };

  typedef U8G2_SSD1309_128X64_NONAME0_F_SW_I2C U8G2_OLED;

  class GUIContainer;

  class GUIComponent {
    public:
      struct Options {
        PixelPartition I_PartitionVertical = {0, 0};
        PixelPartition I_PartitionHorizontal = {0, 0};
      };
    
    public:
      GUIComponent(const Options& Init) : M_PartitionHorizontal(Init.I_PartitionHorizontal), M_PartitionVertical(Init.I_PartitionVertical) {}

      virtual ~GUIComponent() = default;

      GUIComponent(GUIComponent&&) = default;
			GUIComponent& operator=(GUIComponent&&) = default;

			GUIComponent(const GUIComponent&) = default;
			GUIComponent& operator=(const GUIComponent&) = default;

      virtual void RenderComponent(U8G2_OLED* F_OLED) {}
      virtual void TickComponent(U8G2_OLED* F_OLED) {}

    protected:
      PixelPartition M_PartitionVertical = {0, 0};
      PixelPartition M_PartitionHorizontal = {0, 0};

  };

  class GUIPage {
    public:
      struct Options {
        String I_PageName = {"EMPTY"};
      };

    public:
      GUIPage(const Options& Init) : M_PageName(Init.I_PageName) {}

      virtual ~GUIPage() = default;

      GUIPage(GUIPage&&) = default;
			GUIPage& operator=(GUIPage&&) = default;

			GUIPage(const GUIPage&) = delete;
			GUIPage& operator=(const GUIPage&) = delete;

      void RenderPage (U8G2_OLED* F_OLED) {
        Serial.println(M_ComponentManager.size());
        F_OLED->clearBuffer(); 
        for (auto& CurrentComponent : M_ComponentManager) {
          CurrentComponent->RenderComponent(F_OLED);
        }
        F_OLED->sendBuffer(); 
      }
      void TickPage (U8G2_OLED* F_OLED) {
        for (auto& CurrentComponent : M_ComponentManager) {
          CurrentComponent->TickComponent(F_OLED);
        }
      }

      void AddComponent (GUIComponent* F_NewComponent) {
        M_ComponentManager.push_back(F_NewComponent);
      }

      String GetName() const {
        return M_PageName;
      }
      
    private:
      String M_PageName = {"EMPTY"};

      std::vector<GUIComponent*> M_ComponentManager;
  };

  class GUIContainer {
    public:
      struct Options {
        unsigned int I_PIN_SCL = {0};
        unsigned int I_PIN_SDA = {0};

        GUIInput* I_GUIInput;
      };

    public:
      GUIContainer(const Options& Init) : M_OLED(U8G2_R0, Init.I_PIN_SCL, Init.I_PIN_SDA), M_GUIInput(Init.I_GUIInput) {}

      GUIContainer(GUIContainer&&) = default;
			GUIContainer& operator=(GUIContainer&&) = default;

			GUIContainer(const GUIContainer&) = delete;
			GUIContainer& operator=(const GUIContainer&) = delete;

      void Init() {
        M_OLED.begin();

        M_OLED.setFont(u8g2_font_7x13_tf);
        M_OLED.setFontRefHeightExtendedText();
        M_OLED.setDrawColor(1);
        M_OLED.setFontPosTop();
        M_OLED.setFontDirection(0);
      }

      void RenderCurrentPage() {
        if (M_PageIndex < M_PageManager.size()) {
          Serial.print("drawn page with compnt size: ");
          M_PageManager[M_PageIndex].RenderPage(&M_OLED);
        } else {
          Serial.println("failed to draw");
        }
      }
      void TickCurrentPage() {
        if (M_PageIndex < M_PageManager.size()) {
          M_PageManager[M_PageIndex].TickPage(&M_OLED); 
        } else {
          Serial.println("failed to tick");
        }
      }

      void AddPage(GUIPage F_NewPage) {
        M_PageManager.push_back(std::move(F_NewPage));
      }
      GUIPage* GetPage(const String& F_PageName) {
        auto it = std::find_if(
          M_PageManager.begin(), 
          M_PageManager.end(),
          [&F_PageName](const GUIPage& CurrentPage) {
            return CurrentPage.GetName() == F_PageName;
          }
        );

        return (it != M_PageManager.end()) ? &(*it) : nullptr;
      }
      
    private:
      unsigned int PIN_SCL = {0};
      unsigned int PIN_SDA = {0};

      U8G2_OLED M_OLED;

      std::vector<GUIPage> M_PageManager;
      unsigned int M_PageIndex = {0};

      GUIInput* M_GUIInput = {nullptr};
  };
}