#ifndef HEADER_gubg_env_Variables_hpp_ALREADY_INCLUDED
#define HEADER_gubg_env_Variables_hpp_ALREADY_INCLUDED

#include <string>
#include <map>

namespace gubg { namespace env {

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

} }

#endif
