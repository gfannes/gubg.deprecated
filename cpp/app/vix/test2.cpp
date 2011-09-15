#include "CtrlLib/CtrlLib.h"
#include <iostream>
using namespace Upp;
using namespace std;

#define LAYOUTFILE <vix/vix.lay>
#include <CtrlCore/lay.h>

class VixApplication: public WithMainLayout<TopWindow>
{
    public:
        VixApplication()
        {
            CtrlLayout(*this, "The vi-inspired file explorer");
            Sizeable();
        }
        virtual bool Key(dword key, int count)
        {
            cout << "Key " << key << " - 0x" << hex << key << dec << " (" << (char)key << ") was pressed " << count << " times" << endl;
            return true;
        }
};

GUI_APP_MAIN
{
    VixApplication().Run();
}
