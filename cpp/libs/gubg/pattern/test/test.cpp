#include "pattern/Command.hpp"

#include "testing/Testing.hpp"
#define GUBG_LOG
#define LOG_LEVEL Info
#include "logging/Log.hpp"
#include <vector>
using namespace std;

typedef gubg::pattern::ICommand<bool> ICommand;
typedef gubg::pattern::Command<ICommand, int> Command;
class Increment: public Command
{
    public:
        Increment(int &count): Command(count){}
        static ICommand::Ptr create(int &count){return ICommand::Ptr(new Increment(count));}

        virtual bool execute()
        {
            LOG_S(Increment::execute, "Value before: " << receiver_);
            ++receiver_;
            LOG_M("Value after: " << receiver_);
            return true;
        }
        virtual bool undo()
        {
            LOG_S(Increment::undo, "Value before: " << receiver_);
            --receiver_;
            LOG_M("Value after: " << receiver_);
            return true;
        }
};
class Decrement: public Command
{
    public:
        Decrement(int &count): Command(count){}
        static ICommand::Ptr create(int &count){return ICommand::Ptr(new Decrement(count));}

        virtual bool execute()
        {
            LOG_S(Decrement::execute, "Value before: " << receiver_);
            --receiver_;
            LOG_M("Value after: " << receiver_);
            return true;
        }
        //We don't implement undo() for Decrement to test its default throwing behaviour
};

int main()
{
    LOG_S(main);

    {
        LOG_S(Command);
        TEST_TAG(Command);
        int count = 0;

        typedef vector<ICommand::Ptr> Commands;
        Commands commands;
        commands.push_back(Increment::create(count));
        commands.push_back(Increment::create(count));
        commands.push_back(Decrement::create(count));
        TEST_EQ(0, count);

        for (auto command = commands.begin(); command != commands.end(); ++command)
        {
            TEST_TRUE((*command)->execute());
        }
        TEST_EQ(1, count);

        for (auto command = commands.begin(); command != commands.end(); ++command)
        {
            try
            {
                TEST_TRUE((*command)->undo());
            }
            catch (gubg::NotImplemented &) { }
        }
        TEST_EQ(-1, count);
    }

    return 0;
}