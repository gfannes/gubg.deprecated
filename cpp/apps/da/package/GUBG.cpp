//#define GUBG_LOG
#include "da/package/GUBG.hpp"
#include "da/Arduino.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/env/Util.hpp"
#include <string>
using namespace da::package;
using namespace gubg::file;
using namespace std;

GUBG::GUBG(const File &base):
    base_(base),
    libsDir_(base),
    arduinoDir_(base),
    appsDir_(base)
{
    libsDir_ << "cpp/libs";
    arduinoDir_ << "cpp/arduino";
    appsDir_ << "cpp/apps";

    File gubg(libsDir_);
    gubg << "gubg";
    forest_.add(gubg, {"cpp", "hpp"});

    File iup(libsDir_);
    iup << "iup";
    forest_.add(iup, {"cpp", "hpp"});

    File garf(arduinoDir_);
    garf << "garf";
    forest_.add(garf, {"cpp", "hpp"});

    File da(appsDir_);
    da << "da";
    forest_.add(da, {"cpp", "hpp"});
}

bool GUBG::exists() const
{
    return gubg::file::exists(libsDir_) && gubg::file::exists(arduinoDir_) && gubg::file::exists(appsDir_);
}

da::ReturnCode GUBG::resolveHeader(File &resolvedHeader, SourceFiles &sisterFiles, const File &partial)
{
    MSS_BEGIN(ReturnCode, resolveHeader, partial.name());

    //We eat the boost headers when we are building for arduino
    if (compileSettings_.targetPlatform == Arduino)
    {
        File p = partial;
        string root;
        if (p.popRoot(root))
            MSS_Q(root != "boost", RecognisedHeader);
    }

    File root;
    MSS_Q(forest_.resolve(resolvedHeader, root, partial, 1), UnknownHeader);

    {
        string bn;
        MSS_Q(root.popBasename(bn), UnknownHeader);
        if (bn == "gubg")
        {
            if (compileSettings_.targetPlatform == Any)
            {
                compileSettings_.targetPlatform = Host;
                linkSettings_.targetPlatform = Host;
            }
            compileSettings_.includePaths.insert(libsDir_);
        }
        else if (bn == "iup")
        {
            compileSettings_.includePaths.insert(libsDir_);
            compileSettings_.includePaths.insert(File("/home/gfannes/sdks/iup/include"));
            compileSettings_.includePaths.insert(File("/home/gfannes/sdks/cd/include"));
            linkSettings_.libraryPaths.insert(File("/home/gfannes/sdks/iup/lib/Linux35_64"));
            linkSettings_.libraryPaths.insert(File("/home/gfannes/sdks/cd/lib/Linux35_64"));
            linkSettings_.libraries.insert("iup");
            linkSettings_.libraries.insert("iup_pplot");
            linkSettings_.libraries.insert("iupcontrols");
            linkSettings_.libraries.insert("iupcd");
            linkSettings_.libraries.insert("cd");
        }
        else if (bn == "garf")
        {
            MSS_Q(compileSettings_.targetPlatform == Any, UnknownHeader);
            compileSettings_.targetPlatform = Arduino;
            compileSettings_.includePaths.insert(arduinoDir_);
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
            if (linkSettings_.targetPlatform != Arduino)
            {
                string arduinoBase;
                if (gubg::env::expand(arduinoBase, "$GUBG_ARDUINO/hardware/arduino/cores/arduino"))
                    for (auto base: {"main.cpp", "wiring.c", "wiring_digital.c", "wiring_analog.c", "wiring_pulse.c", "WMath.cpp", "HardwareSerial.cpp", "Print.cpp", "WString.cpp", "new.cpp", "Stream.cpp", "WInterrupts.c"})
                    {
                        File f(arduinoBase);
                        f << base;
                        sisterFiles.insert(f);
                    }
            }
            linkSettings_.targetPlatform = Arduino;
        }
        else if (bn == "da")
            compileSettings_.includePaths.insert(appsDir_);
        else
            MSS_QL(UnknownHeader);
    }

    auto sisterFile = resolvedHeader;
    sisterFile.setExtension("cpp");
    if (forest_.contains(sisterFile))
    {
        switch (compileSettings_.targetPlatform)
        {
            case Any:
                sisterFiles.insert(sisterFile);
                break;
            case Arduino:
                if (sisterFile.name().find("msgpack") != string::npos)
                    sisterFiles.insert(sisterFile);
                break;
        }
    }

    MSS_END();
}
