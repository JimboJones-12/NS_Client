#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <vector>
#include <utility>

//////////////////// //////////////////// ////////////////////
//////////////////// //([{ LCD GUI }])// ////////////////////
//////////////////// ////////////////// ////////////////////

namespace GUI {

  //////////////////// ////////////////// ////////////////////
  //////////////////// //([{ Input }])// ////////////////////
  //////////////////// //////////////// ////////////////////

  class GUIInput {
    public:

      //([{ Button State Enum }])//

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

      //([{ Constructor }])//

      GUIInput() {}

      //--
      //--

      //([{ Move Constructor & Operator }])//

      GUIInput(GUIInput&&) = default;
			GUIInput& operator=(GUIInput&&) = default;

      //--
      //--

      //([{ Copy Constructor & Operator }])//

			GUIInput(const GUIInput&) = delete;
			GUIInput& operator=(const GUIInput&) = delete;

      //--
      //--

      //([{ Getter Functions }])//

      State GetState() const {
        return ButtonState;
      }

      //--
      //--

      //([{ Setter Functions }])//

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
  
  //--
  //--

  //////////////////// //////////////////////////////// ////////////////////
  //////////////////// //([{ Forward Declaration }])// ////////////////////
  //////////////////// ////////////////////////////// ////////////////////

  class GUIContainer;
  
  //--
  //--

  //////////////////// ///////////////////////////// ////////////////////
  //////////////////// //([{ GUI Main Classes }])// ////////////////////
  //////////////////// /////////////////////////// ////////////////////

  ////////////////////////////////
	//([{ GUI Component Class }])//
	//////////////////////////////

  class GUIComponent {
    public:

      //([{ Options Struct }])//

      struct Options {
        unsigned int I_LCDColumn = {0};

        unsigned int I_LCDRow = {0};
        unsigned int I_LCDStringWidth = {0};
      };
    
    public:

      //([{ Constructor }])//

      GUIComponent(const Options& Init) : M_LCDColumn(Init.I_LCDColumn), M_LCDRow(Init.I_LCDRow), M_LCDStringWidth(Init.I_LCDStringWidth) {}

      //--
      //--

      //([{ Destructor }])//
      
      virtual ~GUIComponent() = default;

      //--
      //--

      //([{ Move Constructor & Operator }])//

      GUIComponent(GUIComponent&&) = default;
			GUIComponent& operator=(GUIComponent&&) = default;

      //--
      //--

      //([{ Copy Constructor & Operator }])//

			GUIComponent(const GUIComponent&) = default;
			GUIComponent& operator=(const GUIComponent&) = default;

      //--
      //--

      //([{ Polymorphic Functions }])//

      virtual void RenderComponent(GUIContainer& Container) {}
      virtual void TickComponent(GUIContainer& Container) {}

      //--
      //--

      //([{ Getter Functions }])//

      unsigned int GetLCDRow() const {
        return M_LCDRow;
      } 
      unsigned int GetLCDColumn() const {
        return M_LCDColumn;
      }
      unsigned int GetLCDStringWidth() const {
        return M_LCDStringWidth;
      }

    protected:

      //([{ LCD Positioning }])//

      unsigned int M_LCDRow = {0};
      unsigned int M_LCDColumn = {0};
      unsigned int M_LCDStringWidth = {0};

      //--
      //--

      //([{ LCD Display Data }])//
  };
  
  //--
  //--

  ///////////////////////////
	//([{ GUI Page Class }])//
	/////////////////////////

  class GUIPage {
    public:

      //([{ Options Struct }])//

      struct Options {
        String I_PageName = {"EMPTY"};
      };

    public:

      //([{ Constructor }])//

      GUIPage(const Options& Init) : M_PageName(Init.I_PageName) {}

      //--
      //--

      //([{ Destructor }])//
      
      virtual ~GUIPage() = default;

      //--
      //--

      //([{ Move Constructor & Operator }])//

      GUIPage(GUIPage&&) = default;
			GUIPage& operator=(GUIPage&&) = default;

      //--
      //--

      //([{ Copy Constructor & Operator }])//

			GUIPage(const GUIPage&) = delete;
			GUIPage& operator=(const GUIPage&) = delete;

      //--
      //--

      //([{ Polymorphic Functions }])//

      void RenderPage (GUIContainer& Container) {
        for (auto& CurrentComponent : M_ComponentManager) {
          CurrentComponent.RenderComponent(Container);
        }
      }
      void TickPage (GUIContainer& Container) {
        for (auto& CurrentComponent : M_ComponentManager) {
          CurrentComponent.TickComponent(Container);
        }
      }

      void AddComponent (const GUIComponent& NewComponent) {
        M_ComponentManager.push_back(NewComponent);
        std::sort(
          M_ComponentManager.begin(), 
          M_ComponentManager.end(),
          [](const GUIComponent& FirstComponent, const GUIComponent& SecondComponent) {
            if (FirstComponent.GetLCDRow() != SecondComponent.GetLCDRow()) {
                return FirstComponent.GetLCDRow() < SecondComponent.GetLCDRow();
            }
            return FirstComponent.GetLCDColumn() < SecondComponent.GetLCDColumn();
          }
        );
      }
      
    private:
    
      //([{ Name }])//

      String M_PageName = {"EMPTY"};

      //--
      //--

      //([{ Component Manager }])//

      std::vector<GUIComponent> M_ComponentManager;
  };

  //--
  //--

  ////////////////////////////////
	//([{ GUI Container Class }])//
	//////////////////////////////

  class GUIContainer {
    public:

      //([{ Options Struct }])//

      struct Options {
        int I_ContainerLCDAdress = (0x27);
        int I_ContainerLCDColumns = (16);
        int I_ContainerLCDRows = (2);

        GUIInput& I_GUIInput;
      };

    public:

      //([{ Constructor }])//

      GUIContainer(const Options& Init) : M_LCD(Init.I_ContainerLCDAdress, Init.I_ContainerLCDColumns, Init.I_ContainerLCDRows), M_GUIInput(&Init.I_GUIInput) {}

      //--
      //--

      //([{ Move Constructor & Operator }])//

      GUIContainer(GUIContainer&&) = default;
			GUIContainer& operator=(GUIContainer&&) = default;

      //--
      //--

      //([{ Copy Constructor & Operator }])//

			GUIContainer(const GUIContainer&) = delete;
			GUIContainer& operator=(const GUIContainer&) = delete;

      //--
      //--

      //([{ Page Display Functions }])//

      void RenderCurrentPage() {
        if (M_PageManager.size() >= M_PageIndex - 1U) {
          M_PageManager[M_PageIndex].RenderPage(*this); 
        }
      }
      void TickCurrentPage() {
        if (M_PageManager.size() >= M_PageIndex - 1U) {
          M_PageManager[M_PageIndex].TickPage(*this); 
        }
      }

      //--
      //--

      //([{ Page Management Functions }])//

      void AddPage(GUIPage NewPage) {
        M_PageManager.push_back(std::move(NewPage));
      }

      //--
      //--

      //([{ LCD Management Functions }])//

      void PrintToLCD(unsigned int ColumnPosition, unsigned int RowPosition, String PayLoad) {
        M_LCD.setCursor(RowPosition, ColumnPosition);
        M_LCD.print(PayLoad);
      }
      void ClearLCD() {
        M_LCD.clear();
      }
      
    private:

      //([{ LCD Object }])//

      LiquidCrystal_I2C M_LCD;

      //--
      //--

      //([{ Page Management }])//

      std::vector<GUIPage> M_PageManager;
      unsigned int M_PageIndex = {0};

      GUIInput* M_GUIInput = {nullptr};
  };
}