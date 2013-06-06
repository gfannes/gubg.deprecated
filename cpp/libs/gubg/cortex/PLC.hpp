#ifndef HEADER_gubg_cortex_PLC_hpp_ALREADY_INCLUDED
#define HEADER_gubg_cortex_PLC_hpp_ALREADY_INCLUDED

#include "gubg/cortex/IColumn.hpp"
#include <map>

namespace gubg
{
    namespace cortex
    {
        //A piecewise linear column
        class PLC: public IColumn
        {
            public:
                PLC(time::Span span):
                    span_(span){}

                void add(time::Offset offset, Value value)
                {
                    if (offset < -span || offset > span)
                        return;
                    valuePerOffset_[offset] = value;
                }

                void process(const Elapse elapse)
                {
                    const auto minSpan = -span_;
                    for (auto it = valuePerOffset_.begin(); it != valuePerOffset_.end();)
                    {
                        if (it->first < minSpan)
                            it = valuePerOffset_.erase(it);
                        else
                        {
                            it->first -= elapse;
                            ++it;
                        }
                    }
                }

                //IColumn interface
                virtual Value value(time::Offset offset, time::Span span)
                {
                    if (valuePerOffset_.empty())
                        return Value();
                    auto it = valuePerOffset_.lower_bound(offset - span/2);//Not-less than
                    const auto e = valuePerOffset_.upper_bound(offset + span/2);//Greater than
                    time::Offset pt = it->first;
                    Value pv = it->second;
                    Value res;
                    for (++it; it != e; ++it)
                    {
                        const auto dt2 = 0.5*(it->first - pt);
                        const auto &v = it->second;
                        pt = it->first;
                        res.p += dt2*(v.p - pv.p);
                        pv = it->second;
                    }
                    return res;
                }

            private:
                typedef std::map<time::Offset, Value> ValuePerOffset;
                ValuePerOffset valuePerOffset_;
        };
    }
}

#endif
