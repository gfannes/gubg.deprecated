#include "gubg/mss/mss_nostl.hpp"

enum class ReturnCode
{
    MSS_DEFAULT_CODES,
    Foo, Bar, FuBar,
};

bool fooOK_ = true;
ReturnCode foo()
{
    MSS_BEGIN(ReturnCode);
    MSS(fooOK_, Foo);
    MSS_END();
}

bool barOK_ = true;
ReturnCode bar()
{
    MSS_BEGIN(ReturnCode);
    MSS(barOK_, Bar);
    MSS_END();
}

ReturnCode foobar()
{
    MSS_BEGIN(ReturnCode);
    MSS(foo());
    MSS(bar());
    MSS_END();
}

ReturnCode fubar()
{
    MSS_BEGIN(ReturnCode);
    MSS_L(FuBar);
    MSS_END();
}

bool main_()
{
    MSS_BEGIN(bool);
    MSS(foobar());
    MSS(ReturnCode::OK == foobar());
    fooOK_ = false;
    MSS(ReturnCode::Foo == foobar());
    barOK_ = false;
    MSS(ReturnCode::Foo == foobar());
    fooOK_ = true;
    MSS(ReturnCode::Bar == foobar());
    barOK_ = true;
    MSS(foobar());
    MSS(ReturnCode::FuBar == fubar());
    MSS_END();
}
int main()
{
    return (main_() ? 0 : -1);
}
