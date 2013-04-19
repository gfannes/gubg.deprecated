#ifndef HEADER_gubg_build_Options_hpp_ALREADY_INCLUDED
#define HEADER_gubg_build_Options_hpp_ALREADY_INCLUDED

#include <map>
#include <vector>
#include <string>

namespace gubg
{
    namespace build
    {
        class Options
        {
            public:
                enum class ReturnCode {OK, UnknownName};
                typedef std::string Name;
                typedef std::string Value;
                typedef std::vector<Value> Values;
                typedef std::map<Name, Values> Map;

                void add(const Name &, const Value &);
                ReturnCode getValues(Values &, const Name &) const;

            private:
                Map map_;
        };
    }
}

#endif
