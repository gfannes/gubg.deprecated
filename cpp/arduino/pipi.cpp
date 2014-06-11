#include "gubg/file/Descriptor.hpp"
#include "gubg/String.hpp"
#include <queue>

enum class ReturnCode {MSS_DEFAULT_CODES, };

#define GUBG_MODULE_ "pipi"
#include "gubg/log/begin.hpp"
namespace pipi { 

    class Model: public gubg::file::Select
    {
        public:
            Model(unsigned short port, gubg::file::File flamingo)
            {
                croco_listen_ = Descriptor::listen(port);
                add(croco_listen_, AccessMode::Read);
                flamingo_listen_ = Descriptor::listen(flamingo, AccessMode::ReadWrite);
                add(flamingo_listen_, AccessMode::ReadWrite);
            }

        private:
            typedef gubg::file::Descriptor Descriptor;
            typedef gubg::file::AccessMode AccessMode;
            typedef gubg::file::EventType EventType;

            virtual void select_ready(Descriptor descr, EventType eventType)
            {
                MSS_BEGIN(void, descr << " " << to_hr(eventType));
                switch (eventType)
                {
                    case EventType::Open:
                        L("Someone is trying to connect");
                        if (false) {}
                        else if (descr == croco_listen_)
                        {
                            MSS(!croco_descr_.valid());
                            MSS(descr.accept(croco_descr_));
                            MSS(add(croco_descr_, AccessMode::Read));
                            L("Croco is connected");
                        }
                        else if (descr == flamingo_listen_)
                        {
                            MSS(!flamingo_descr_.valid());
                            MSS(descr.accept(flamingo_descr_));
                            MSS(add(flamingo_descr_, AccessMode::Read));
                            L("Flamingo is connected");
                        }
                        break;
                    case EventType::Read:
                        L("Descriptor is ready for reading");
                        {
                            std::string buffer(1024, '\0');
                            {
                                const auto rc = descr.read(buffer);
                                if (rc == gubg::file::ReturnCode::PeerClosedConnection)
                                {
                                    L("Peer closed the connection, I will throw him out of my select");
                                    MSS(erase(descr, AccessMode::Read));
                                    MSS_RETURN_OK();
                                }
                                MSS(rc);
                                MSS(!buffer.empty());
                            }

                            if (false) {}
                            else if (descr == croco_descr_)
                            {
                                L("I received " << buffer.size() << " bytes from croco: " << gubg::toHex(buffer));
                                messages_4_flamingo_.push(buffer);
                                if (messages_4_flamingo_.size() == 1)
                                {
                                    L("First message is queue, adding flamingo_descr_ to Write set");
                                    MSS(add(flamingo_descr_, AccessMode::Write));
                                }
                            }
                            else if (descr == flamingo_descr_)
                            {
                                L("I received " << buffer.size() << " bytes from flamingo: " << buffer);
                            }
                        }
                        break;
                    case EventType::Write:
                        L("Descriptor is ready for writing");
                        MSS(descr == flamingo_descr_);
                        if (messages_4_flamingo_.empty())
                        {
                            L("No more messages to send, removing flamingo from Write set");
                            MSS(erase(flamingo_descr_, AccessMode::Write));
                            MSS_RETURN_OK();
                        }
                        {
                            size_t nrWritten = 0;
                            auto &msg = messages_4_flamingo_.front();
                            MSS(flamingo_descr_.write(nrWritten, msg));
                            if (nrWritten == msg.size())
                            {
                                messages_4_flamingo_.pop();
                                if (messages_4_flamingo_.empty())
                                {
                                    L("No more messages to send, removing flamingo from Write set");
                                    MSS(erase(flamingo_descr_, AccessMode::Write));
                                    MSS_RETURN_OK();
                                }
                            }
                            else
                                msg = msg.substr(nrWritten);
                        }
                        break;
                }
                MSS_END();
            }

            Descriptor croco_listen_;
            Descriptor croco_descr_;
            Descriptor flamingo_listen_;
            Descriptor flamingo_descr_;

            typedef std::queue<std::string> Messages;
            Messages messages_4_flamingo_;
    };

} 

ReturnCode main_()
{
    MSS_BEGIN(ReturnCode);

    pipi::Model model(1234, gubg::file::File("/dev/ttyACM0"));
    while (MSS_IS_OK(model.process()))
    {
    }

    MSS_END();
}
int main()
{
    return (MSS_IS_OK(main_()) ? 0 : -1);
}
#include "gubg/log/end.hpp"
