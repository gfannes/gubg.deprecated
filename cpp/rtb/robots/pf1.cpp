#include "rtb/Commander.hpp"
#include "gubg/file/File.hpp"
using namespace std;

namespace 
{
    class Commander: public rtb::Commander_crtp<Commander>
    {
        public:
            Commander():
                Commander_crtp(gubg::file::File(__FILE__).basename()){}

        private:
    };
}

int main()
{
    Commander commander;
    while (commander.process())
    {
    }

    return 0;
}
