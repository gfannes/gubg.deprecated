//!@#adoc
//Trace tracking tool
#include "ttt/Codes.hpp"
#include "gubg/OptionParser.hpp"
#include "gubg/file/Descriptor.hpp"
#include "gubg/trace/DTO.hpp"
#include "gubg/msgpack/Factory.hpp"
#include "gubg/Testing.hpp"
#include "gubg/Platform.hpp"
#include <iostream>
using namespace std;
using namespace gubg;
using namespace ttt;

#define GUBG_MODULE_ "ttt"
#include "gubg/log/begin.hpp"
namespace ttt { 
    void finalize(const string &msg, int ret)
    {
        cout << msg << endl;
        exit(ret);
    }
    struct Options
    {
        unsigned short port = 1234;
    };
    std::ostream &operator<<(std::ostream &os, const Options &options)
    {
        os << "Port: " << options.port << endl;
        return os;
    }
	typedef std::map<ThreadId, std::string> MsgPerTid;
	typedef std::map<ProcessId, MsgPerTid> InfoPerPid;
	InfoPerPid infoPerPid;
	class Factory: public gubg::msgpack::Factory_crtp<Factory, std::string, 15>
	{
		public:
			gubg::msgpack::Wrapper<std::string> msgpack_createObject(AttributeId aid, TypeId tid)
			{
				switch (tid)
				{
					case gubg::trace::dto::TypeIds::OpenScope: return wrap(os);
					case gubg::trace::dto::TypeIds::CloseScope: return wrap(cs);
				}
				return gubg::msgpack::Wrapper<std::string>();
			}
			void msgpack_createdObject(AttributeId aid, TypeId tid)
			{
				switch (tid)
				{
					case gubg::trace::dto::TypeIds::OpenScope:
						//cout << os << endl;
						infoPerPid[os.pid][os.tid] = os.msg;
						break;
					case gubg::trace::dto::TypeIds::CloseScope:
						//cout << cs << endl;
						break;
				}
			}
			void msgpack_set(AttributeId id, gubg::msgpack::Nil_tag) {S();L("???");}
			void msgpack_set(AttributeId id, const std::string &str) {S();L("???");}
			void msgpack_set(AttributeId id, long) {S();L("???");}

			gubg::trace::dto::OpenScope os;
			gubg::trace::dto::CloseScope cs;
	};
	class FactoryMgr
	{
		public:
			void process(const std::string &str, file::Descriptor d)
			{
				//SSS("Processing " << str.size() << " bytes");
				factoryPerDesc_[d].process(str);
			}
		private:
			typedef std::map<file::Descriptor, Factory> FactoryPerDesc;
			FactoryPerDesc factoryPerDesc_;
	};
	FactoryMgr factoryMgr;
	class Select: public file::Select
	{
		public:
			virtual void select_ready(file::Descriptor d, file::EventType et)
			{
				S();L(d << " is ready for some action: " << to_hr(et));
				switch (et)
				{
					case file::EventType::Open:
						{
							file::Descriptor c;
							d.accept(c);
							add(c, file::AccessMode::Read);
						}
						break;
					case file::EventType::Read:
						{
							std::string str(1024, '?');
							d.read(str);
							L(testing::toHex(str));
							factoryMgr.process(str, d);
						}
						break;
					case file::EventType::Close:
						{
							erase(d, file::AccessMode::ReadWrite);
						}
						break;
				}
			}
		private:
	};
} 

ReturnCode main_(int argc, char **argv)
{
	MSS_BEGIN(ReturnCode);
	Options options;
	{
		OptionParser parser("trace tracking tool");
		parser.addSwitch("-h", "--help", "Displays this help", [&](){finalize(parser.help(), 0);});
		parser.addMandatory("-p", "--port", "TCP port to listen", [&](const string &port){options.port = atoi(port.c_str());});
		auto args = OptionParser::createArgs(argc, argv);
		parser.parse(args);
	}
	L(options);

	auto d = file::Descriptor::listen(options.port);
	MSS(d.valid());
	Select s;
	s.add(d, file::AccessMode::Read);
	typedef std::chrono::system_clock Clock;
	auto timeout = Clock::now();
	while (true)
	{
		S();
		s.process(std::chrono::seconds(1));
		if (Clock::now() >= timeout)
		{
			timeout = Clock::now() + std::chrono::seconds(3);
			for (auto p: infoPerPid)
			{
				cout << p.first << endl;
				for (auto p2: p.second)
					cout << "\t" << p2.first << ": " << p2.second << endl;
			}
		}
	}

	MSS_END();
}
int main(int argc, char **argv)
{
	const auto rc = main_(argc, argv);
	switch (rc)
	{
	}
	cout << "Everthing went OK" << endl;
	return 0;
}
#include "gubg/log/end.hpp"
