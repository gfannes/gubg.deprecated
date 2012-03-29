#include "gubg/pimpl.hpp"
#include "gubg/mss.hpp"
#include "gubg/testing/Testing.hpp"
#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include <memory>
using namespace std;

enum class ReturnCode
{
    MSS_DEFAULT_CODES, set
};

class API
{
    private:
        struct PIMPL
        {
            ReturnCode set(int i);
            int get() const;
            //A non-const method can still be accessed from a const API
            void ff();
            int multiply(int n);

            //Pure data. This has to be visible though, else, the templates cannot work
            int data;
        };

    public:
        API();
        void clear(){pimpl_.reset();}

        //ReturnCode-based API can be forwarded
        PIMPL_FORWARD_MSS(set);

        //Normal function, own forwarding
        PIMPL_FORWARD_RAW(int, get);

        //Watch-out, pimpl is a pointer: API::ff is const, but API::Pimpl::ff is _not_
        PIMPL_FORWARD_RAW(void, ff);

        PIMPL_FORWARD_RAW(int, multiply);
};

//Implementation
API::API(): pimpl_(new Pimpl) { }

//Pure implementation
ReturnCode API::Pimpl::set(int i){data = i; return ReturnCode::OK;}
int API::Pimpl::get() const {return data;}
void API::Pimpl::ff() {}
int API::Pimpl::multiply(int n){return data*n;}

int main()
{
    LOG_S(main);
    TEST_TAG(main);
    MSS_BEGIN(int);
    API api;
    TEST_OK(api);
    TEST_OK(api.set(42));
    TEST_EQ(42, api.get());
    TEST_EQ(84, api.multiply(2));
    api.clear();
    TEST_KO(api);
    //TODO::Doesn't work yet, should be fixed some day, see WIP in testing.hpp
    //TEST_EQ(ReturnCode::PimplError, api.set(2));
    TEST_KO(api.set(2));
    try
    {
        api.multiply(2);
        api.get();
    }
    catch (const std::string &str) {LOG_M("Catched string " << str);}
    MSS_END();
}
