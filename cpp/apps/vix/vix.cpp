#ifdef VIX_WITH_UPP

#include "vix/controller/VixApplication.hpp"

GUI_APP_MAIN
{
    VixApplication().Run();
}

#endif



#ifdef VIX_WITH_QT

#include "controller/VixApplication.hpp"
#include <QApplication>

int main(int argc, char **argv)
{
    vix::VixApplication application(argc, argv);

    return application.exec();
}

#endif