#ifndef vix_model_Selection_hpp
#define vix_model_Selection_hpp

#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
#include <vector>
#include <string>

namespace vix
{
    namespace model
    {
        struct File
        {
            File(){}
            File(const boost::filesystem::path &p):
                path(p.root_path().string()),
                name(p.filename().string()){}

            bool operator<(const File &rhs) const {return std::make_pair(path, name) < std::make_pair(rhs.path, rhs.name);}

            bool isHidden() const {return !name.empty() && name[0] == '.';}

            std::string path;
            std::string name;
        };
        typedef std::vector<File> Files;

        class Selection
        {
            public:
                Selection(const std::string &path);

                std::string path() const {return path_.string();}
                Files getFiles() const;

                void setFilter(const std::string &);

            private:
                boost::filesystem::path path_;
                typedef boost::scoped_ptr<boost::regex> Regex;
                Regex filter_;
        };
    }
}

#endif
