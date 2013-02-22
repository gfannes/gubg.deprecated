#ifndef HEADER_iup_Callback_hpp_ALREADY_INCLUDED
#define HEADER_iup_Callback_hpp_ALREADY_INCLUDED

//This code is based on iupplusplus.h which can be found here:
//http://pulkomandy.tk/projects/thomson/browser/Thomson/elec/CrO2/software/iupplusplus.h

//This is the original copyright statement:
/* CrO2 datassette emulator
 * Copyright 2012, Adrien Destugues <pulkomandy@pulkomandy.ath.cx>
 *
 * Distributed under the terms of the MIT licence.
 */

//Usage:
//Callback<Gui>::create(menu_open, "ACTION", this, &Gui::doStuff);
//Callback<Gui, int, int>::create(toggle, "ACTION", this, &Gui::doMoreStuff);

#include "iup/iup.hpp"

namespace iup
{

    template <class Handler, typename ret = int, typename... Args>
        class Callback
        {
            typedef ret(Handler::*T)(Args... args);

            public:
            static int destroy(Ihandle* that)
            {
                Callback* call = (Callback*)IupGetAttribute(that, "LCALLBACK");
                delete call;
                return IUP_DEFAULT;
            }

            static void create(Ihandle* handle, const char* name, Handler* self, T what)
            {
                Callback* cb = new Callback(self, what);
                IupSetAttribute(handle, "LCALLBACK", (char*)cb);
                IupSetCallback(handle, name, (Icallback)Callback::call);
                IupSetCallback(handle, "LDESTROY_CB", Callback::destroy);
            }

            static ret call(Ihandle* that, Args... args)
            {
                Callback* call = (Callback*)IupGetAttribute(that, "LCALLBACK");
                return ((call->self)->*(call->what))(args...);
            }

            private:
            Callback(Handler* self, T what)
            {
                this->self = self;
                this->what = what;
            }

            Callback(); // do not use

            Handler* self;
            T what;
        };
}

#endif
