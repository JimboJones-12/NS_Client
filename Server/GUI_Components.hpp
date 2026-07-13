#include "GUI.hpp"

namespace GUI {

  class Title : public GUIComponent {
    public:

      //([{ Options Struct }])//

      struct Options {
        unsigned int I_LCDColumn = {0};
        unsigned int I_LCDRow = {0};
        unsigned int I_LCDStringWidth = {0};

        String I_TitleString = {"EMPTY"};
      };

    public:

      //([{ Constructor }])//

      Title(const Options& Init) : GUIComponent({Init.I_LCDColumn, Init.I_LCDRow, Init.I_LCDStringWidth}) {
        M_PayLoadString = "";
        for (int i = 0; i < M_LCDStringWidth; i ++) {
          M_PayLoadString += Init.I_TitleString.charAt(i);
        }
      }

      //--
      //--

      //([{ Mobe Constructor & Operator }])//

      Title(Title&&) = default;
			Title& operator=(Title&&) = default;

      //--
      //--

      //([{ Copy Constructor & Operator }])//

			Title(const Title&) = default;
			Title& operator=(const Title&) = default;

      //--
      //--

      //([{ Polymorphic Functions }])//

      void RenderComponent(GUIContainer& Container) override final {
        Container.PrintToLCD(M_LCDRow, M_LCDColumn, M_PayLoadString);
      }
      void TickComponent(GUIContainer& Container) override final {
        
      }

    private:
      String M_PayLoadString = {"EMPTY"};
  };
}
