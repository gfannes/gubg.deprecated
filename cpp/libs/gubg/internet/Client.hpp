#ifndef gubg_internet_Client_hpp
#define gubg_internet_Client_hpp

#include "gubg/internet/Socket.hpp"

namespace gubg
{
    namespace internet
    {
        class Client
        {
            public:
                Client(const std::string &ip, const int port):
                    ip_(ip),
                    port_(port){}

                template <typename Handler>
                    ReturnCode createConnection(std::shared_ptr<Handler> &handler)
                    {
                        MSS_BEGIN(ReturnCode);
                        Socket socket;
                        MSS(socket.connect(ip_, port_));
                        handler.reset(new Handler(socket));
                        MSS_END();
                    }

            private:
                const std::string ip_;
                const int port_;
        };
    }
}

#endif
