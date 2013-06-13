#ifndef HEADER_gubg_cortex_PLC_hpp_ALREADY_INCLUDED
#define HEADER_gubg_cortex_PLC_hpp_ALREADY_INCLUDED

#include "gubg/cortex/IColumn.hpp"
#include <map>
#include <algorithm>

#define GUBG_MODULE_ "PLC"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace cortex
    {
        //A piecewise linear column
        class PLC: public IColumn
        {
            public:
                PLC(time::Span span, time::Span pulse):
                    span_(span), pulse_(pulse),
                    now_(time::Clock::now()){}

                void add(time::Offset offset, Value value)
                {
                    if (offset < -span_ || offset > span_)
                        return;
                    valuePerPoint_[now_ + offset] = value;
                }

                void process(const time::Elapse elapse)
                {
                    now_ += elapse;
                    const auto minPoint = now_-span_;
                    for (auto it = valuePerPoint_.begin(); it != valuePerPoint_.end();)
                    {
                        if (it->first < minPoint)
                            it = valuePerPoint_.erase(it);
                        else
                            ++it;
                    }
                }

                //IColumn interface
                virtual Value value(time::Offset offset, time::Span span)
                {
                    if (valuePerPoint_.empty())
                        return Value();
                    const auto point = now_ + offset;
                    auto it = valuePerPoint_.lower_bound(point - span/2);//Not-less than
                    const auto e = valuePerPoint_.upper_bound(point + span/2);//Greater than
                    Value res;
                    for (; it != e; ++it)
                    {
                        const Value &l = it->second;
                        res.v += l.v*l.p;//sum v*p
                        res.p += l.p;//sum p
                    }
                    if (res.p > 0)
                    {
                        res.v /= res.p;
                        const auto r = span.count()/pulse_.count();
                        res.p /= std::max<decltype(r)>(r, 1);
                        res.p = std::min(res.p, 1.0);
                    }
                    return res;
                }

            private:
                const time::Span span_;
                const time::Span pulse_;
                time::Point now_;

                typedef std::map<time::Point, Value> ValuePerPoint;
                ValuePerPoint valuePerPoint_;
        };
    }
}
#include "gubg/log/end.hpp"

#endif
