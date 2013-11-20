#ifndef HEADER_gubg_internet_Client_hpp_ALREADY_INCLUDED
#define HEADER_gubg_internet_Client_hpp_ALREADY_INCLUDED

#include "gubg/internet/Socket.hpp"

#define GUBG_MODULE "Client"
#include "gubg/log/begin.hpp"
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
#include "gubg/log/end.hpp"

#endif
