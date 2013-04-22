#include "da/link/Linker.hpp"
#include "da/Arduino.hpp"
#include <sstream>
#include <stdlib.h>
#include <cassert>
using namespace da;
using namespace da::compile;
using namespace std;

#define GUBG_MODULE "Linker"
#include "gubg/log/begin.hpp"
Linker::Linker(ExeType exeType):
    exeType_(exeType){}
ReturnCode Linker::operator()(const ExeFile &exe, const ObjectFiles &objects)
{
    MSS_BEGIN(ReturnCode);

    //Prepare the command to be executed
    ostringstream cmd;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        switch (settings.targetPlatform)
        {
            case Any:
            case Host:
                switch (exeType_)
                {
                    case ExeType::Debug:
                        cmd << "g++ -std=c++11 -g -pthread -o ";
                        break;
                    case ExeType::Release:
                        cmd << "g++ -pthread -o ";
                        break;
                    default: assert(false); break;
                }
                break;
            case Arduino:
                if (arduino::isUno())
                    cmd << "avr-g++ -Os -w -Wl,--gc-sections -mmcu=atmega328p -o ";
                else if (arduino::isMega())
                    cmd << "avr-g++ -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega2560 -o ";
                else
                    cmd << "UNEXPECTED ARDUINO";
                break;
            default:
                cmd << "UNKNOWN TARGET PLATFORM ";
                break;
        }
        cmd << exe.name();
        for (const auto &obj: objects)
            cmd << " " << obj.name();
        for (const auto &option: settings.linkOptions)
            cmd << " " << option;
        for (const auto &libraryPath: settings.libraryPaths)
            cmd << " -L" << libraryPath.name();
        for (const auto &lib: settings.libraries)
            cmd << " -l" << lib;
    }

    //Execute the compilation command
    verbose(cmd.str());
    MSS(::system(cmd.str().c_str()) == 0, LinkingFailed);

    //Run our new creation
    switch (settings.targetPlatform)
    {
        case Any:
        case Host:
            {
                cmd.str("");
                cmd << "./" << exe.name();
                int res;
                {
                    SSS(cmd.str());
                    verbose("---------------------------------------------Start------------------------------------------------------");
                    res = ::system(cmd.str().c_str());
                    verbose("---------------------------------------------Stop-------------------------------------------------------");
                }
                if (res != 0)
                {
                    verbose("  =>  ERROR");
                    MSS_L(RunFailed);
                }
                verbose("  =>  OK");
            }
            break;

        case Arduino:
            {
                cmd.str("");
                auto hex = exe;
                hex.setExtension("hex");
                cmd << "avr-objcopy -R .eeprom -O ihex " << exe.name() << " " << hex.name();
                verbose(cmd.str());
                MSS(::system(cmd.str().c_str()) == 0, AvrObjCopyFailed);

                cmd.str("");
                if (arduino::isUno())
                    cmd << "avrdude -c arduino -p m328p -P /dev/ttyACM0 -U flash:w:" << hex.name();
                else if (arduino::isMega())
                    cmd << "avrdude -c stk500v2 -b 115200 -p atmega2560 -P /dev/ttyACM0 -U flash:w:" << hex.name();
                else
                    cmd << "UNEXPECTED ARDUINO";
                verbose(cmd.str());
                MSS(::system(cmd.str().c_str()) == 0, AvrDudeFailed);
            }
            break;
    }

    MSS_END();
}
