#ifndef vix_model_Selection_hpp
#define vix_model_Selection_hpp

#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
#include "boost/signals2.hpp"
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
                typedef boost::signals2::signal<void ()> UpdateSignal;

                Selection(const std::string &path);

                std::string path() const {return path_.string();}
                void setPath(const std::string &);
                void setSelected(const std::string &);

                static const int InvalidIX = -2;
                void getFiles(Files &, int &selectedIX) const;
                bool gotoSelected();
                enum class Direction {Up, Down};
                bool move(Direction);

                void setFilter(const std::string &);

                boost::signals2::connection connect(const UpdateSignal::slot_type &subscriber);

            private:
                UpdateSignal updated_;

                //This is the primary data for the set of files_
                boost::filesystem::path path_;
                typedef boost::scoped_ptr<boost::regex> Regex;
                Regex filter_;
                //
                Files files_;
                void updateFiles_();

                //We first try to match selected_. If that fails, we take something as close as possible to selectedIX_
                std::string selected_;
                int selectedIX_;
                void updateSelection_();
        };
    }
}

#endif
