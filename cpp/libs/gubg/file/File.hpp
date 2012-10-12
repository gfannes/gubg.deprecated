#ifndef gubg_file_File_hpp
#define gubg_file_File_hpp

#include <string>

namespace gubg
{
    namespace file
    {
        class File
        {
            public:
                enum Type {Unknown, Directory, Regular, Symbolic, FIFO};

                File()                                   :type_(Unknown){};
                File(const std::string & name)           :type_(Unknown), name_(name){}
                File(      std::string &&name)           :type_(Unknown), name_(std::move(name)){}
                File(const std::string & name, Type type):type_(type),    name_(name){}
                File(      std::string &&name, Type type):type_(type),    name_(std::move(name)){}
                File(const File &dir, const std::string & name, Type type):type_(type)
                {
                    if (dir.name().back() == '/')
                        name_ = dir.name() + name;
                    else
                        name_ = dir.name() + '/' + name;
                }

                //Getters
                std::string name() const {return name_;}
                Type type()        const {return type_;}

                //Setters
                File &setName(const std::string & name){name_ = name;            return *this;}
                File &setName(      std::string &&name){name_ = std::move(name); return *this;}
                File &setType(Type type)               {type_ = type;            return *this;}

            private:
                Type type_;
                std::string name_;
        };
    }
}

#endif
