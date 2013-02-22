#include "iup/Dialog.hpp"
#include "iup/Callback.hpp"
#include "iup/Canvas.hpp"
#include "gubg/l.hpp"

namespace 
{
    class Canvas: public iup::Canvas_crtp<Canvas>
    {
        public:
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
        private:
    };

    class Main: public iup::Widget
    {
        public:
            Main()
            {
                canvas.setAttribute("SIZE", "100x200");
                addChild(canvas);
            }
        protected:
            virtual Ihandle *create_()
            {
                L("Main create_");
                /*
                   Ihandle *lbl, *vb, *dlg;

                   lbl=IupLabel("oeuao aouaoeu");

                   vb=IupVbox(lbl, cnv_, 0);
                   IupSetAttribute(vb, "GAP", "10");
                   IupSetAttribute(vb, "MARGIN", "10x10");
                   IupSetAttribute(vb, "ALIGNMENT", "ACENTER");
                   */
                return canvas;
            }
        private:
            Canvas canvas;
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
