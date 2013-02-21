#ifndef HEADER_iup_iup_hpp_ALREADY_INCLUDED
#define HEADER_iup_iup_hpp_ALREADY_INCLUDED

#include "iup.h"

namespace iup
{

    class IUP
    {
        public:
            IUP(int &argc, char **&argv)
            {
                IupOpen(&argc, &argv);
            }
            ~IUP()
            {
                IupClose();
            }
    };
}

#endif
