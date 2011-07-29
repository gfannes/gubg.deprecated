#ifndef vix_controller_Instruction_hpp
#define vix_controller_Instruction_hpp

#include <string>

namespace vix
{
    //A command line instruction
    class Instruction
    {
        public:
            Instruction(const std::string &);

            bool isValid() const;
            std::string command() const {return command_;}
            std::string options() const {return options_;}

        private:
            void parseInstruction_(const std::string &instruction);
            bool parseCommand_(const std::string &command, const std::string &instruction);
            std::string command_;
            std::string options_;
    };
}

#endif
