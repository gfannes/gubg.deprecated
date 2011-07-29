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

        class Selection;

        class Selections
        {
            public:
                typedef boost::signals2::signal<void (Selection *)> UpdateSignal;

                Selections();
                virtual ~Selections();

                boost::signals2::connection connect(const UpdateSignal::slot_type &subscriber);

                bool empty() const;
                Selection *current() const;
                void setCurrent(int ix);
                void addSelection(const std::string &path);

            private:
                friend class Selection;
                UpdateSignal updated_;
                typedef std::vector<Selection*> Selections_;
                Selections_ selections_;
                int current_;
        };

        class Selection
        {
            private:
                friend class Selections;
                Selection(Selections &selections, const std::string &path);

            public:
                Path path() const {return path_;}
                void setPath(Path);
                void setSelected(const std::string &selected);
                std::string getSelected() const;

                std::vector<Selection*> selections();

                static const int InvalidIX = -2;
                void getFiles(Files &, int &selectedIX) const;
                Activation activateSelected(Action);
                enum class Direction {Up, Down};
                bool move(Direction);

                void setFilter(const std::string &);

            private:
                Selections &selections_;

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
