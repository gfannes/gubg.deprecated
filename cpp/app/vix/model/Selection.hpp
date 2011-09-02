#ifndef vix_model_Selection_hpp
#define vix_model_Selection_hpp

#include "model/FileSystem.hpp"
#include "threading/Queue.hpp"
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
                Selections();
                virtual ~Selections();

                typedef boost::signals2::signal<void (Selection *)> UpdateSignal;
                boost::signals2::connection connect(const UpdateSignal::slot_type &subscriber);

                bool empty() const;
                Selection *current() const;
                int currentIX() const;
                int nrModels() const;
                void setCurrent(int ix);
                void addSelection(const std::string &path);
                void deleteSelection(int ix);

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
                ~Selection();
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

                void setNameFilter(const std::string &);
                std::string getNameFilter() const;

                void setRecursiveMode(bool);
                bool getRecursiveMode() const;

            private:
                Selections &selections_;

                //This is the primary data for the set of files_
                Path path_;
                std::string nameFilter_;
                typedef boost::scoped_ptr<boost::regex> Regex;
                Regex reNameFilter_;
                //
                Files files_;
                void updateFiles_();

                //We first try to match selected_. If that fails, we take something as close as possible to selectedIX_
                std::string selected_;
                int selectedIX_;
                void updateSelection_(const std::string &selected = "");

                bool recursiveMode_;

                //All updating of internal data goes via a message queue
                struct Message
                {
                    typedef std::unique_ptr<Message> Ptr;
                    std::string nameFilter;
                    std::string selected;
                };
                typedef gubg::threading::Queue<Message> QueueT;
                QueueT queue_;
                void consumer_();
                boost::thread consumerThread_;
        };
    }
}

#endif
