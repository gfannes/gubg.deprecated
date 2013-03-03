#include "iup/Dialog.hpp"
#include "iup/Callback.hpp"
#include "iup/Canvas.hpp"
#include "iup/Box.hpp"
#include "iup/Button.hpp"
#include "iup/Label.hpp"
#include "gubg/l.hpp"

namespace 
{
    class Canvas: public iup::Canvas_crtp<Canvas>
    {
        public:
            Canvas()
            {
                setAttribute("SIZE", "100x200");
            }
            void canvas_repaint()
            {
                L("Canvas repaint");
                cdCanvasActivate(canvas_);
                cdCanvasClear(canvas_);
                cdCanvasForeground(canvas_, CD_BLUE);
                cdCanvasBox(canvas_, 10, 100, 10, 100);
                cdCanvasLineStyle(canvas_, CD_DASHED);
                cdCanvasLine(canvas_, 0, 0, 100, 100);
            }
    };
    class Button: public iup::Button_crtp<Button>
    {
        public:
            Button(Widget &w):widget(w)
            {
                setLabel("Hello button");
            }
            void button_clicked()
            {
                L("I you click me again, I will explode");
                IupRedraw(widget, 1);
            }
        private:
            Widget &widget;
    };

    class Main: public iup::VBox
    {
        public:
            Main():label("Let's label"), button(canvas)
            {
                add(label);
                add(button);
                add(canvas);

                setAttribute("GAP", "10");
                setAttribute("MARGIN", "10x10");
                setAttribute("ALIGNMENT", "ACENTER");
            }
        private:
            iup::Label label;
            Canvas canvas;
            Button button;
    };
}

int main(int argc, char **argv)
{
    iup::IUP raii(argc, argv);
    iup::Dialog<Main> dialog;
    dialog.setTitle("This is the title");
    dialog.run();
    return 0;
}
