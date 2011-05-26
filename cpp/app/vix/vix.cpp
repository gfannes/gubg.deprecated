#include "controller/VixApplication.hpp"
#include <QApplication>

int main(int argc, char **argv)
{
    vix::VixApplication application(argc, argv);

    return application.exec();
}
