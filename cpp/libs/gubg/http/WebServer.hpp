#ifndef HEADER_gubg_http_WebServer_hpp_ALREADY_INCLUDED
#define HEADER_gubg_http_WebServer_hpp_ALREADY_INCLUDED

#include "gubg/http/Request.hpp"
#include "gubg/http/Response.hpp"
#include "gubg/file/Descriptor.hpp"

#define GUBG_MODULE_ "WebServer"
#include "gubg/log/begin.hpp"
namespace gubg { namespace http { 

    //Receiver should implement webserver_process(Response &, const Request &)
    template <typename Receiver>
        class WebServer_crtp
        {
            public:
                WebServer_crtp(unsigned short port = 80): port_(port), select_(static_cast<Receiver&>(*this)) {}

                template <typename Duration>
                    void process(Duration duration)
                    {
                        S();
                        if (!acceptor_.valid())
                        {
                            L("Creating the acceptor_");
                            //The first time, we just make the acceptor descriptor
                            acceptor_ = file::Descriptor::listen(port_);
                            select_.add(acceptor_, file::AccessMode::Read);
                        }
                        select_.process(duration);
                    }

            private:
                const unsigned short port_;
                file::Descriptor acceptor_;
                struct Info
                {
                    std::string read;
                    std::string write;
                };
                struct Select: file::Select
                {
                    virtual void select_ready(file::Descriptor d, file::EventType et)
                    {
                        S();
                        switch (selectReady_(d, et))
                        {
                            case ReturnCode::OK:
                                break;
                            case ReturnCode::ResponseWasSent:
                                infoPerDesc_.erase(d);
                                erase(d, file::AccessMode::ReadWrite);
                                break;
                            default:
                                infoPerDesc_.erase(d);
                                erase(d, file::AccessMode::ReadWrite);
                                break;
                        }
                    }
                    ReturnCode selectReady_(file::Descriptor d, file::EventType et)
                    {
                        MSS_BEGIN(ReturnCode);
                        switch (et)
                        {
                            case file::EventType::Open:
                                {
                                    L("Received a new connection");
                                    file::Descriptor client;
                                    MSS(d.accept(client));
                                    add(client, file::AccessMode::Read);
                                }
                                break;
                            case file::EventType::Read:
                                {
                                    std::string buf(1024, '?');
                                    switch (const auto rc = d.read(buf))
                                    {
                                        case file::ReturnCode::OK:
                                            {
                                                L("Receiving data:" << std::endl << buf);
                                                auto &info = infoPerDesc_[d];
                                                info.read += buf;

                                                Request request;
                                                switch (const auto rc2 = request.parse(info.read))
                                                {
                                                    case ReturnCode::NotEnoughData: MSS_RETURN_OK(); break;
                                                    default: MSS(rc2); break;
                                                }

                                                Response response;
                                                receiver_.webserver_process(response, const_cast<const Request &>(request));

                                                MSS(response.valid());
                                                info.write = response.serialize();
                                                L("Sending back response:" << std::endl << info.write);
                                                erase(d, file::AccessMode::Read);
                                                add(d, file::AccessMode::Write);
                                            }
                                            break;
                                        case file::ReturnCode::PeerClosedConnection:
                                            MSS_Q(rc); break;
                                        default:
                                            MSS(rc); break;
                                    }
                                }
                                break;
                            case file::EventType::Write:
                                {
                                    L("I can write some data");
                                    auto &info = infoPerDesc_[d];
                                    size_t nr;
                                    MSS(d.write(nr, info.write));
                                    //Round trip is over
                                    MSS_Q(nr < info.write.size(), ResponseWasSent);
                                    auto tmp = info.write.substr(nr);
                                    info.write.swap(tmp);
                                }
                                break;
                        }
                        MSS_END();
                    }
                    typedef std::map<file::Descriptor, Info> InfoPerDesc;
                    InfoPerDesc infoPerDesc_;
                    Receiver &receiver_;
                    Select(Receiver &receiver): receiver_(receiver) {}
                };
                Select select_;
        };

} } 
#include "gubg/log/end.hpp"
#endif
