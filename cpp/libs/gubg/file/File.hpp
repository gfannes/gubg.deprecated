#ifndef HEADER_gubg_file_File_hpp_ALREADY_INCLUDED
#define HEADER_gubg_file_File_hpp_ALREADY_INCLUDED

#include "gubg/string_algo.hpp"
#include <string>
#include <vector>

#include "gubg/l.hpp"

namespace gubg
{
    namespace file
    {
        class File
        {
            public:
                enum Type {Unknown, Directory, Regular, Symbolic, FIFO};
                static const char Delimiter = '/';

                File()                                   :type_(Unknown){};
                File(const std::string & name)           :type_(Unknown), name_(name)           {canonicalize_();}
                File(      std::string &&name)           :type_(Unknown), name_(std::move(name)){canonicalize_();}
                File(const std::string & name, Type type):type_(type),    name_(name)           {canonicalize_();}
                File(      std::string &&name, Type type):type_(type),    name_(std::move(name)){canonicalize_();}

		void clear()
		{
			type_ = Unknown;
			name_.clear();
		}

                //Getters
                std::string name() const {return name_;}
                Type type()        const {return type_;}
                std::string extension() const
                {
                    if (type() != Regular && type() != Unknown)
                        return "";
                    auto ixdot = name_.rfind('.');
                    if (ixdot == std::string::npos)
                        return "";
                    auto ixdelim = name_.rfind(Delimiter);
                    if (ixdelim != std::string::npos && ixdot < ixdelim)
                        return "";
                    return name_.substr(ixdot+1);
                }
                std::string basename() const
                {
                    auto ix = name_.rfind(Delimiter);
                    return name_.substr((ix == std::string::npos ? 0 : ix+1));
                }

                //Setters
                File &setName(const std::string & name){name_ = name;            canonicalize_(); return *this;}
                File &setName(      std::string &&name){name_ = std::move(name); canonicalize_(); return *this;}
                File &setType(Type type)               {type_ = type;            return *this;}
                File &setExtension(const std::string &ext)
                {
                    if (type() == Regular || type() == Unknown)
                    {
                        const auto oldExt = extension();
                        name_.resize(name_.size() - oldExt.size());
                        if (name_.empty() || name_.back() != '.')
                            name_ += '.';
                        name_ += ext;
                    }
                    return *this;
                }

                //Append a part to the current File
                File &operator<<(const std::string &name)
                {
                    if (name.empty())
                        return *this;

                    if (name_.empty())
                    {
                        name_ = name;
                        return *this;
                    }

                    if (name_.back() == Delimiter)
                    {
                        if (name.front() == Delimiter)
                            name_ += name.substr(1);
                        else
                            name_ += name;
                    }
                    else
                    {
                        if (name.front() == Delimiter)
                            name_ += name;
                        else
                            name_ += Delimiter + name;
                    }
                    return *this;
                }

                File relative(const File &wd) const
                {
                    const auto nameParts = string_algo::split<std::vector>(name(), Delimiter);
                    const auto wdParts = string_algo::split<std::vector>(wd.name(), Delimiter);
                    auto nameIt = nameParts.begin();
                    auto wdIt = wdParts.begin();
                    File file;
                    while (nameIt != nameParts.end() && wdIt != wdParts.end())
                    {
                        if (*nameIt != *wdIt)
                            break;
                        ++nameIt;
                        ++wdIt;
                    }
                    //Step up the required levels, if any
                    for (; wdIt != wdParts.end(); ++wdIt)
                        file << "..";
                    //Step down, if any
                    for (; nameIt != nameParts.end(); ++nameIt)
                        file << *nameIt;
                    return file;
                }

            private:
		void canonicalize_()
		{
			for (auto &ch: name_)
				if (ch == '\\')
					ch = '/';
		}

                Type type_;
                std::string name_;
        };
    }
}

#endif
