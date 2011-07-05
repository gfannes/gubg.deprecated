#ifndef vix_model_Command_hpp
#define vix_model_Command_hpp

namespace vix
{
    class ICommand
    {
        public:
            virtual bool operator()() = 0;
    };

    //Navigation-related commands
    class Open: public ICommand
    {
        public:
            virtual bool operator()();
    };
    class Search: public ICommand
    {
        public:
            virtual bool operator()();
    };

    //Tab-related commands
    class NewTab: public ICommand
    {
        public:
            virtual bool operator()();
    };
    class CloseTab: public ICommand
    {
        public:
            virtual bool operator()();
    };
    class FocusTab: public ICommand
    {
        public:
            virtual bool operator()();
    };
}

#endif
