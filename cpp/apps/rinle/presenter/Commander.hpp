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

					if (cmd_ == " ")
						return toggleRubber_();

					if (cmd_ == "n")
						return move_(model::Forward);
					if (cmd_ == "o")
						return move_(model::Backward);
					if (cmd_ == "t")
						return move_(model::In);
					if (cmd_ == "e")
						return move_(model::Out);

					if (cmd_ == "b")
						return navmode_(model::ByToken);
					if (cmd_ == "m")
						return navmode_(model::ByScope);
					if (cmd_ == "w")
						return navmode_(model::ByLine);

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
				CommandPtr toggleRubber_()
				{
					return CommandPtr(new Command([&](){outer_.toggleRubber_();}));
				}
				CommandPtr move_(model::Direction dir)
				{
					return CommandPtr(new Command([&, dir](){outer_.move_(dir);}));
				}
				CommandPtr navmode_(model::NavigatorMode mode)
				{
					return CommandPtr(new Command([&, mode](){outer_.navmode_(mode);}));
				}

				Outer &outer_;
				std::string cmd_;
		};

} }
#include "gubg/log/end.hpp"

#endif
