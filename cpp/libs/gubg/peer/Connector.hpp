#ifndef HEADER_gubg_peer_Connector_hpp_ALREADY_INCLUDED
#define HEADER_gubg_peer_Connector_hpp_ALREADY_INCLUDED

#include "gubg/peer/Codes.hpp"
#include <string>

#define GUBG_MODULE "Connector"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace peer
    {
        template <typename Receiver>
            class Connector_crtp
            {
                public:
                    typedef std::string Msg;

                    class Address
                    {
                        public:
                            enum Type {Unknown, Tcp, File};

                            Address():type_(Unknown){}
                            Address(const std::string &ip, const int port):
                                type_(Tcp), ip_(ip), port_(port){}
                            Address(const std::string &filename):
                                type_(File), filename_(filename){}

                            bool isTcp() const {return type_ == Tcp;}
                            bool isFile() const {return type_ == File;}

                            bool getIpPort(std::string &ip, int &port)
                            {
                                if (!isTcp())
                                    return false;
                                ip = ip_;
                                port = port_;
                                return true;
                            }
                            bool getFilename(std::string &filename)
                            {
                                if (!isFile())
                                    return false;
                                filename = filename_;
                                return true;
                            }

                        private:
                            Type type_;
                            std::string ip_;
                            int port_;
                            std::string filename_;
                    };

                    class IConnection
                    {
                        public:
                            virtual void receivedData(const Msg &) = 0;
                            ReturnCode send(const Msg &msg)
                            {
                                MSS_BEGIN(ReturnCode);
                                MSS_END();
                            }
                            void process()
                            {
                            }

                        protected:
                            Address address_;
                    };

                    ReturnCode connect(IConnection &connection, const Address &address)
                    {
                        MSS_BEGIN(ReturnCode);
                        MSS_END();
                    }

                    ReturnCode watch(const Address &address)
                    {
                        MSS_BEGIN(ReturnCode);
                        MSS_END();
                    }

                    //Default event point that gets called when a new connection is received
                    IConnection *connector_NewConnection(const Address &) { return 0; }

                private:
                    Receiver &receiver_(){return static_cast<Receiver&>(*this);}
            };
    }
}
#include "gubg/log/end.hpp"

#endif
