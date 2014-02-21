#include "gubg/Cartouche.hpp"
#include "gubg/parse/Line.hpp"
using namespace std;

#define GUBG_MODULE_ "Cartouche"
#include "gubg/log/begin.hpp"
namespace  { 
    const string marker("!@#");
} 

namespace gubg { 

    Cartouche::Cartouches Cartouche::parse(const string &content)
    {
        S();
        Cartouches res;
        Cartouche *current = 0;
        string pre;
        for (auto &l: line::split(content))
        {
            SS(l.txt);
            if (!current)
            {
                L("No cartouche detected yet");
                const auto ix = l.txt.find(marker);
                if (ix == string::npos)
                    //Still no cartouche detected
                    continue;
                L("We detected a cartouche, add one and initialize it");
                res.push_back(Cartouche());
                current = &res.back();
                current->tag = l.txt.substr(ix+marker.size());
                pre = l.txt.substr(0, ix);
            }
            else
            {
                L("We are detecting a cartouche");
                if (pre != l.txt.substr(0, pre.size()))
                {
                    LL("The pre does not match anymore, the previous cartouche is closed now");
                    current = 0;
                    continue;
                }
                L("This line is part of the cartouche, add it, together with its endline marker");
                current->content.append(l.txt.substr(pre.size()));
                current->content.append(to_s(l.end));
            }
        }
        return res;
    }

}
#include "gubg/log/end.hpp"
