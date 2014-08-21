#include "da/package/Arduino.hpp"
#include "da/Arduino.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/env/Util.hpp"
#include <string>
using namespace da::package;
using namespace gubg::file;
using namespace std;

#define GUBG_MODULE_ "Arduino"
#include "gubg/log/begin.hpp"
Arduino::Arduino(const File &base):
    base_(base){}

bool Arduino::exists() const
{
    SS(base_.name());
    return gubg::file::exists(base_);
}

da::ReturnCode Arduino::resolveHeader(File &resolvedHeader, SourceFiles &sisterFiles, const File &partial)
{
    MSS_BEGIN(ReturnCode, partial.name());

    MSS_Q(partial.name() == "Arduino.h", UnknownHeader);

    {
        string hdr;
        MSS(gubg::env::expand(hdr, "$GUBG_ARDUINO/hardware/arduino/cores/arduino/Arduino.h"));
        resolvedHeader.setName(hdr);
    }

    MSS(compileSettings_.targetPlatform == Any || compileSettings_.targetPlatform == Platform::Arduino);
    if (compileSettings_.targetPlatform == Any)
    {
        compileSettings_.targetPlatform = Platform::Arduino;
        string str;
        if (gubg::env::expand(str, "$GUBG_ARDUINO/hardware/arduino/cores/arduino"))
            compileSettings_.includePaths.insert(File(str));
        if (arduino::isUno())
        {
            if (gubg::env::expand(str, "$GUBG_ARDUINO/hardware/arduino/variants/standard"))
                compileSettings_.includePaths.insert(File(str));
        }
        if (arduino::isMega())
        {
            if (gubg::env::expand(str, "$GUBG_ARDUINO/hardware/arduino/variants/mega"))
                compileSettings_.includePaths.insert(File(str));
        }
        if (gubg::env::expand(str, "$GUBG_ARDUINO/libraries"))
             compileSettings_.includePaths.insert(File(str));
        if (gubg::env::expand(str, "$GUBG_ARDUINO/libraries/Wire/utility"))
             compileSettings_.includePaths.insert(File(str));
        if (linkSettings_.targetPlatform != Platform::Arduino)
        {
            string arduinoBase;
            if (gubg::env::expand(arduinoBase, "$GUBG_ARDUINO/hardware/arduino/cores/arduino"))
                for (auto base: {"main.cpp", "wiring.c", "wiring_digital.c", "wiring_analog.c", "wiring_pulse.c", "WMath.cpp", "HardwareSerial.cpp", "Print.cpp", "WString.cpp", "new.cpp", "Stream.cpp", "WInterrupts.c"})
                {
                    File f(arduinoBase);
                    f << base;
                    sisterFiles.insert(f);
                }
#if 1
            if (gubg::env::expand(arduinoBase, "$GUBG_ARDUINO/libraries"))
                for (auto base: {"Servo/Servo.cpp", /*Wire/Wire.cpp*/})
                {
                    File f(arduinoBase);
                    f << base;
                    sisterFiles.insert(f);
                }
#endif
        }
        linkSettings_.targetPlatform = Platform::Arduino;
    }
    MSS(compileSettings_.targetPlatform == Platform::Arduino);

    MSS_END();
}
