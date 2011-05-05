#ifndef gubg_OptionParser_h
#define gubg_OptionParser_h

#include <string>
#include <functional>
#include <map>
#include <list>
#include <sstream>

namespace gubg
{
    class OptionParser
    {
        public:
            OptionParser(const std::string &caption);

            typedef std::list<std::string> Args;
            static Args convertArgs(int argc, char **argv);

            bool parse(Args &args, bool stripExe = true);

            template <typename Lambda>
                void addSwitch(const std::string &shortHand, const std::string &longHand, const std::string &description, Lambda lambda)
                {
                    switchCallbacks_[shortHand] = lambda;
                    switchCallbacks_[longHand] = lambda;
                    addHelpLine_(shortHand, longHand, description);
                }

            //longHand can be of the form: "--<option> <name>"
            template <typename Lambda>
                void addOptional(const std::string &shortHand, const std::string &longHand, const std::string &description, Lambda lambda)
                {
                    optionalCallbacks_[shortHand] = lambda;
                    {
                        std::string longOption = longHand;
                        size_t pos = longOption.find(' ');
                        if (pos != std::string::npos)
                            longOption.resize(pos);
                        optionalCallbacks_[longOption] = lambda;
                    }
                    addHelpLine_(shortHand, longHand, description);
                }

            //longHand can be of the form: "--<option> <name>"
            template <typename Lambda>
                void addMandatory(const std::string &shortHand, const std::string &longHand, const std::string &description, Lambda lambda)
                {
                    mandatoryCallbacks_[shortHand] = lambda;
                    {
                        std::string longOption = longHand;
                        size_t pos = longOption.find(' ');
                        if (pos != std::string::npos)
                            longOption.resize(pos);
                        mandatoryCallbacks_[longOption] = lambda;
                    }
                    addHelpLine_(shortHand, longHand, description);
                }

            std::string help() const;

        private:
            void addHelpLine_(const std::string &shortHand, const std::string &longHand, const std::string &description);

            std::string caption_;
            std::ostringstream help_;
            typedef std::function<void(void)> VoidCallback;
            typedef std::map<std::string, VoidCallback> VoidCallbacks;
            typedef std::function<void(std::string)> StringCallback;
            typedef std::map<std::string, StringCallback> StringCallbacks;
            VoidCallbacks switchCallbacks_;
            StringCallbacks optionalCallbacks_;
            StringCallbacks mandatoryCallbacks_;
    };
}

#endif
