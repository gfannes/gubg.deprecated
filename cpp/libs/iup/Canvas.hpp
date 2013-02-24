#ifndef HEADER_iup_Canvas_hpp_ALREADY_INCLUDED
#define HEADER_iup_Canvas_hpp_ALREADY_INCLUDED

#include "iup/iup.hpp"
#include "gubg/crtp.hpp"
#include "cd.h"
#include "cdiup.h"

namespace iup
{
    template <typename Receiver>
    class Canvas_crtp: public Widget
    {
        public:
            Canvas_crtp():canvas_(0){}
        protected:
            virtual Ihandle *create_()
            {
                Ihandle *cnv = IupCanvas(0);
                L("Canvas create_ " << cnv);
                iup::Callback<Canvas_crtp>::create(cnv, "ACTION", this, &Canvas_crtp::repaint_);
                return cnv;
            }
            virtual void mapped_()
            {
                L("Canvas mapped");
                cdInitContextPlus();
                cdUseContextPlus(1);
                canvas_ = cdCreateCanvas(CD_IUP, *this);
                cdCanvasForeground(canvas_, CD_BLUE);
                cdCanvasSetAttribute(canvas_, "ANTIALIAS", "0");
                //cdUseContextPlus(0);
            }
        protected:
            GUBG_RECEIVER();
            int repaint_()
            {
                receiver_().canvas_repaint();
                return IUP_DEFAULT;
            }
            cdCanvas *canvas_;
    };
}

#endif
