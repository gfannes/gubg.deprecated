#ifndef HEADER_gubg_Cartouche_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Cartouche_hpp_ALREADY_INCLUDED

#include <string>
#include <map>
#include <vector>

namespace gubg
{
    class Cartouche
    {
        public:
            typedef std::string Path;

            bool parse(const std::string &str);

        private:
            enum Operation {Set, Append, SetIfUnset};
            typedef std::string Value;
            typedef std::pair<std::string, std::string> Filter;
            struct Info
            {
                Operation operation;
                Value value;
                Filter filter;
            };
            typedef std::vector<Info> Infos;
            typedef std::map<Path, Infos> InfosPerPath;
            InfosPerPath infosPerPath_;
    };
}

#endif
