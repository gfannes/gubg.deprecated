#ifndef vix_model_Selection_hpp
#define vix_model_Selection_hpp

#include "model/FileSystem.hpp"
#include "boost/regex.hpp"
#include "boost/signals2.hpp"
#include <vector>
#include <string>

namespace vix
{
    namespace model
    {
        enum class Action {View, Edit, Open, Delete, Copy, Move};
        enum class Activation {Error, Directory, Regular, SomethingElse};

        class Selection
        {
            public:
                typedef boost::signals2::signal<void (Selection *)> UpdateSignal;

                Selection(const std::string &path);

                Path path() const {return path_;}
                void setPath(Path);
                void setSelected(const std::string &selected);

                static const int InvalidIX = -2;
                void getFiles(Files &, int &selectedIX) const;
                Activation activateSelected(Action);
                enum class Direction {Up, Down};
                bool move(Direction);

                void setFilter(const std::string &);

                boost::signals2::connection connect(const UpdateSignal::slot_type &subscriber);

            private:
                UpdateSignal updated_;

                //This is the primary data for the set of files_
                Path path_;
                typedef boost::scoped_ptr<boost::regex> Regex;
                Regex filter_;
                //
                Files files_;
                void updateFiles_();

                //We first try to match selected_. If that fails, we take something as close as possible to selectedIX_
                std::string selected_;
                int selectedIX_;
                void updateSelection_(const std::string &selected = "");
        };
    }
}

#endif
