#ifndef HEADER_rinle_presenter_Commander_hpp_ALREADY_INCLUDED
#define HEADER_rinle_presenter_Commander_hpp_ALREADY_INCLUDED

#include "rinle/Types.hpp"
#include "rinle/model/Types.hpp"
#include "gubg/pattern/SignalSlot.hpp"
#include <string>
#include <iostream>

#define GUBG_MODULE_ "Commander"
#include "gubg/log/begin.hpp"
namespace rinle { namespace presenter {

    template <typename Outer>
        class Commander
        {
            public:
                Commander(Outer &outer): outer_(outer), slot(*this) {}

				gubg::pattern::Signal<std::string> logger;

                void process(char ch)
                {
                    S();L(ch);
					cmd_.push_back(ch);
					auto cmd = createCommand_();
					if (cmd)
					{
						cmd->execute(outer_);
						cmd_.clear();
					}
					{
						std::ostringstream oss; oss << "Command bufer: \"" << cmd_ << "\"";
						logger.emit(oss.str());
					}
				}
				gubg::pattern::Slot<Commander, char> slot;

			private:
				struct Command
				{
					typedef std::shared_ptr<Command> Ptr;
					typedef std::function<void()> Oper;
					Oper oper_;
					Command(Oper oper): oper_(oper) {}
					void execute(Outer &outer)
					{
						oper_();
					}
				};
				typedef typename Command::Ptr CommandPtr;
				CommandPtr createCommand_()
				{
					CommandPtr nothing;
					if (cmd_.empty())
						return nothing;
					if (cmd_.back() == 27)
						return clear_();
					if (cmd_ == "qq")
						return quit_();
					if (cmd_ == "n")
						return move_(model::Forward);
					if (cmd_ == "o")
						return move_(model::Backward);
					if (cmd_ == " ")
						return toggleRubber_();
					if (cmd_ == "t")
						return navmode_(model::ByToken);
					if (cmd_ == "l")
						return navmode_(model::ByToken);
					return nothing;
				}
				CommandPtr clear_()
				{
					return CommandPtr(new Command([&](){cmd_.clear();}));
				}
				CommandPtr quit_()
				{
					return CommandPtr(new Command([&](){std::cout << "User requested to exit. Bye." << std::endl; nana::gui::API::exit();}));
				}
				CommandPtr move_(model::Direction dir)
				{
					CommandPtr cmd;
					switch (dir)
					{
						case model::Forward:
						case model::Backward:
							cmd.reset(new Command([&](){outer_.move_(dir);})); break; 
					}
					return cmd;
				}
				CommandPtr toggleRubber_()
				{
					return CommandPtr(new Command([&](){outer_.toggleRubber_();}));
				}
				CommandPtr navmode_(model::NavigatorMode mode)
				{
					return CommandPtr(new Command([&](){outer_.navmode_(mode);}));
				}

				Outer &outer_;
				std::string cmd_;
		};

} }
#include "gubg/log/end.hpp"

#endif
