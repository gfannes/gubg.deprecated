#ifndef gubg_internet_Socket_hpp
#define gubg_internet_Socket_hpp

#include "gubg/internet/Codes.hpp"
#include "gubg/IOBuffer.hpp"
#include <memory>
#include <string>

namespace gubg
{
    namespace internet
    {
        class Socket
        {
            public:
                typedef gubg::IOBuffer<char> IOBuffer;

                Socket(){}

                ReturnCode bind(int port);
                ReturnCode listen();
                ReturnCode accept(Socket &);

                ReturnCode connect(const std::string &ip, int port);

                ReturnCode send(IOBuffer &);
                //Returns ConnectionWasClosed if the connection was closed by the peer (receiving 0-sized data)
                ReturnCode receive(IOBuffer &);

                ReturnCode shutdown();

            private:
                Socket(int fid);

                ReturnCode createSocket_();

                struct Pimpl;
                std::shared_ptr<Pimpl> pimpl_;
        };
    }
}

#endif
