#ifndef gubg_testing_TestMaster_hpp
#define gubg_testing_TestMaster_hpp

namespace gubg
{
    //Singleton that handles all TestMgr instances and global statistics reporting
    class TestMaster
    {
        public:
            static TestMaster &instance();

        private:
            TestMaster(){}
            TestMaster(const TestMaster &);
            TestMaster &operator=(const TestMaster &);
    };
}

#endif
