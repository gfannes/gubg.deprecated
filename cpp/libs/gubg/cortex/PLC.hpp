#ifndef HEADER_gubg_cortex_PLC_hpp_ALREADY_INCLUDED
#define HEADER_gubg_cortex_PLC_hpp_ALREADY_INCLUDED

#include "gubg/cortex/IColumn.hpp"
#include <map>

#define GUBG_MODULE "PLC"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace cortex
    {
        //A piecewise linear column
        class PLC: public IColumn
        {
            public:
                PLC(time::Span span):
                    span_(span),
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
                    auto pt = it->first;
                    Value pv = it->second;
                    Value res;
                    for (++it; it != e; ++it)
                    {
                        const auto dt = (it->first - pt).count();
                        const auto &v = it->second;
                        pt = it->first;
                        res.p += dt*(v.p + pv.p)/2.0;
                        res.v += dt*(2*(pv.v*pv.p + v.v*v.p) + pv.v*v.p + v.v*pv.p)/6.0;
                        pv = it->second;
                    }
                    if (res.p > 0)
                    {
                        res.v /= res.p;
                        res.p /= span.count();
                    }
                    return res;
                }

            private:
                const time::Span span_;
                time::Point now_;

                typedef std::map<time::Point, Value> ValuePerPoint;
                ValuePerPoint valuePerPoint_;
        };
    }
}
#include "gubg/log/end.hpp"

#endif
