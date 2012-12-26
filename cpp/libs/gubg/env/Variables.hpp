#ifndef gubg_env_Environment_hpp
#define gubg_env_Environment_hpp

#include <string>
#include <map>

namespace gubg
{
    namespace env
    {
        class Variables
        {
            public:
                typedef std::string Variable;
                typedef std::string Value;

                static Variables shell();

                bool get(Value &, const Variable &) const;

            private:
                typedef std::map<Variable, Value> ValuePerVariable;
                ValuePerVariable valuePerVariable_;
        };
    }
}

#endif
