#ifndef aoeuaoeu_Commander_aoeuaoeu
#define aoeuaoeu_Commander_aoeuaoeu

#include "rinle/Types.hpp"
#include "gubg/pattern/Observer.hpp"

#define GUBG_MODULE_ "Commander"
#include "gubg/log/begin.hpp"
namespace rinle
{
	namespace presenter
	{

		class Commander: public gubg::pattern::Observable<Command>
		{
			public:
				Commander(): observer(*this) {}
				gubg::pattern::Observer_ftop<Commander, char> observer;

				void notify(char ch)
				{
					S();L(ch);
					switch (ch)
					{
						case 'n': proceed_(1); break;
						case 'o': proceed_(-1); break;
					}
				}

			private:
				void proceed_(int nrSteps)
				{
					Command command(Command::Proceed);
					command.x = nrSteps;
					notifyObservers(command);
				}
		};
	}
}
#include "gubg/log/end.hpp"

#endif
