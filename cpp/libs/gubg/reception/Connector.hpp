#ifndef HEADER_gubg_reception_Connector_hpp_ALREADY_INCLUDED
#define HEADER_gubg_reception_Connector_hpp_ALREADY_INCLUDED

#include "ev++.h"
#include <thread>

#define GUBG_MODULE_ "Connector"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace reception
    {
        class Connector
        {
            public:
                Connector():
                    thread_(std::ref(*this)){}
                ~Connector()
                {
                    thread_.join();
                }

                void operator()()
                {
                    S();
                    L("Starting the default event loop");
                    ev::default_loop loop;
                }

            private:
                std::thread thread_;
        };
    }
}
#include "gubg/log/end.hpp"

#endif
