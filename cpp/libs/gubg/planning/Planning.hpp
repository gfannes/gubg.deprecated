#ifndef HEADER_gubg_planning_Planning_hpp_ALREADY_INCLUDED
#define HEADER_gubg_planning_Planning_hpp_ALREADY_INCLUDED

#include "gubg/planning/Codes.hpp"
#include "gubg/planning/Line.hpp"
#include "gubg/planning/Resources.hpp"
#include <vector>
#include <map>

#define GUBG_MODULE "Planning"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace planning
    {
        class Planning
        {
            public:
                Resources resources;

                Line &getLine(Line::Name name)
                {
                    auto line = linePerName_.find(name);
                    if (line == linePerName_.end())
                    {
                        linePerName_[name].setName(name);
                        line = linePerName_.find(name);
                    }
                    return line->second;
                }

                ReturnCode run()
                {
					MSS_BEGIN(ReturnCode);
                    TaskRange tr;
                    while (getNextDeadlineToPlan_(tr))
						MSS(planASAP_(tr), CouldNotPlanDeadline);
                    while (getNextRangeToPlan_(tr))
						MSS(planASAP_(tr), CouldNotPlanNormal);
					MSS_END();
                }

                void stream(std::ostream &os) const
                {
                    for (auto &lpn: linePerName_)
                        os << lpn.second;
                }

            private:
                bool getNextDeadlineToPlan_(TaskRange &tr)
                {
                    gubg::OnlyOnce first;
                    for (auto &lpn: linePerName_)
                    {
                        TaskRange ltr;
                        if (lpn.second.getUnplannedRange(ltr) && ltr.back().deadline.isValid())
                        {
                            if (first() || (ltr.back().deadline < tr.back().deadline))
                                tr = ltr;
                        }
                    }
                    return !first();
                }
                bool getNextRangeToPlan_(TaskRange &tr)
                {
                    for (auto &lpn: linePerName_)
                        if (lpn.second.getUnplannedRange(tr))
                            return true;
                    return false;
                }
                ReturnCode planASAP_(TaskRange &tr)
                {
					MSS_BEGIN(ReturnCode);
                    for (auto &task: tr)
						MSS(resources.planASAP(task));
					MSS_END();
                }
                typedef std::map<Line::Name, Line> LinePerName;
                LinePerName linePerName_;
        };
    }
}
namespace std
{
    ostream &operator<<(ostream &os, const gubg::planning::Planning &pl)
    {
        pl.stream(os);
        return os;
    }
}
#include "gubg/log/end.hpp"

#endif
