#ifndef HEADER_rinle_presenter_Commander_hpp_ALREADY_INCLUDED
#define HEADER_rinle_presenter_Commander_hpp_ALREADY_INCLUDED

#include "rinle/Types.hpp"
#include "gubg/pattern/Observer.hpp"

#define GUBG_MODULE_ "Commander"
#include "gubg/log/begin.hpp"
namespace rinle
{
	namespace presenter
	{
        template <typename Outer>
		class Commander: public gubg::pattern::Observable<Command>
		{
			public:
				Commander(Outer &outer): outer_(outer), observer(*this) {}
				gubg::pattern::Observer_ftop<Commander, char> observer;

				void notify(char ch)
				{
					S();L(ch);
					switch (ch)
					{
						case 'n': outer_.proceed_(1); break;
						case 'o': outer_.proceed_(-1); break;
					}
				}

			private:
                Outer &outer_;
		};
	}
}
#include "gubg/log/end.hpp"

#endif
