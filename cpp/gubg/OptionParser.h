#ifndef gubg_OptionParser_h
#define gubg_OptionParser_h

#include <string>
#include <functional>
#include <map>
#include <list>

#define L_ENABLE_DEBUG
#include <debug.hpp>

namespace gubg
{
    class OptionParser
    {
        public:
            OptionParser(const std::string &caption):
                caption_(caption){}

            template <typename Lambda>
            void addSwitch(const std::string &shortHand, const std::string &longHand, const std::string &description, Lambda lambda)
            {
                switchCallbacks_[shortHand] = lambda;
                switchCallbacks_[longHand] = lambda;
            }

            typedef std::list<std::string> Args;
            static Args convertArgs(int argc, char **argv)
            {
                Args args;
                for (int i = 0; i < argc; ++i)
                    args.push_back(argv[i]);
                return args;
            }

            bool parse(Args &args, bool stripExe = true)
            {
                if (stripExe)
                {
                    if (args.empty())
                        return false;
                    args.pop_front();
                }

                //When args is empty, we are certainly done
                while (!args.empty())
                {
                    DEBUG_PRINT("Front: " << args.front());
                    auto it = switchCallbacks_.find(args.front());
                    if (it != switchCallbacks_.end())
                    {
                        (it->second)();
                        args.pop_front();
                    }
#if 0
                    else if (args.front in mandatoryDelegates_)
                    {
                        if (args.length < 2)
                            return false;
                        mandatoryDelegates_[args.front](args[1]);
                        args.popFront;
                        args.popFront;
                    }
                    else if (args.front in optionalDelegates_)
                    {
                        if (args.length < 2 || args[1].length == 0 || '-' == args[1][0])
                        {
                            optionalDelegates_[args.front](null);
                        } else
                        {
                            optionalDelegates_[args.front](args[1]);
                            args.popFront;
                        }
                        args.popFront;
                    }
#endif
                    else
                        break;
                }
                return true;
            }

            std::string help() const
            {
                return caption_;
            }
        private:
            std::string caption_;
            std::map<std::string, std::function<void(void)>> switchCallbacks_;
    };
}

#endif
