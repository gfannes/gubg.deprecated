#include "gubg/Testing.hpp"
#include "gubg/file/Creater.hpp"
using namespace gubg::file;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
namespace 
{
    class Creater: public gubg::file::Creater_crtp<Creater>
    {
        public:
            template <typename Files, typename SI>
            ReturnCode creater_create(Files files, SI si) const
            {
                MSS_BEGIN(ReturnCode);
                for (auto f: files)
                    MSS(write(si, f));
                MSS_END();
            }
        private:
    };
}

int main()
{
    TEST_TAG(main);
    Creater creater;
    TEST_KO(creater.setCache(File("/a/b/c")));
    TEST_OK(creater.setCache(File("/tmp")));
    creater({File("/home/gfannes/blabla")}, "abc");
}
#include "gubg/log/end.hpp"
