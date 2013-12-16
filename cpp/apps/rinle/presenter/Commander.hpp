#ifndef HEADER_rinle_presenter_Commander_hpp_ALREADY_INCLUDED
#define HEADER_rinle_presenter_Commander_hpp_ALREADY_INCLUDED

#include "rinle/Types.hpp"
#include "gubg/pattern/SignalSlot.hpp"

#define GUBG_MODULE_ "Commander"
#include "gubg/log/begin.hpp"
namespace rinle { namespace presenter {

    template <typename Outer>
        class Commander
        {
            public:
                Commander(Outer &outer): outer_(outer), slot(*this) {}

                void process(char ch)
                {
                    S();L(ch);
                    switch (ch)
                    {
                        case 'n': outer_.proceed_(1); break;
                        case 'o': outer_.proceed_(-1); break;
                    }
                }
                gubg::pattern::Slot<Commander, char> slot;

            private:
                Outer &outer_;
        };

} }
#include "gubg/log/end.hpp"

#endif
