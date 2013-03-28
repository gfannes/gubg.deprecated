#ifndef HEADER_gubg_file_File_hpp_ALREADY_INCLUDED
#define HEADER_gubg_file_File_hpp_ALREADY_INCLUDED

#include "gubg/string_algo.hpp"
#include <string>
#include <vector>

namespace gubg
{
    namespace file
    {
        class File
        {
            public:
                enum Type {Unknown, Directory, Regular, Symbolic, FIFO};
                typedef std::string Name;
                static const char Delimiter;// = '/';

                File()                                   :type_(Unknown){};
                File(const Name & name)           :type_(Unknown), name_(name)           {canonicalize_();}
                File(      Name &&name)           :type_(Unknown), name_(std::move(name)){canonicalize_();}
                File(const Name & name, Type type):type_(type),    name_(name)           {canonicalize_();}
                File(      Name &&name, Type type):type_(type),    name_(std::move(name)){canonicalize_();}

                bool operator==(const File &rhs) const
                {
                    return type_ == rhs.type_ && name_ == rhs.name_;
                }
                bool operator<(const File &rhs) const
                {
                    return std::make_pair(name_, type_) < std::make_pair(rhs.name_, rhs.type_);
                }

                void clear()
                {
                    type_ = Unknown;
                    name_.clear();
                }

                //Getters
                Name name() const {return name_;}
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
                Name basename() const
                {
                    auto ix = name_.rfind(Delimiter);
                    return name_.substr((ix == std::string::npos ? 0 : ix+1));
                }

                //Setters
                File &setName(const Name & name){name_ = name;            canonicalize_(); return *this;}
                File &setName(      Name &&name){name_ = std::move(name); canonicalize_(); return *this;}
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

                bool popBasename(std::string &bn)
                {
                    auto ix = name_.rfind(Delimiter);
                    if (ix == std::string::npos)
                        return false;
                    bn = name_.substr(ix+1);
                    name_.resize(ix);
                    return true;
                }
                bool popBasename()
                {
                    auto ix = name_.rfind(Delimiter);
                    if (ix == std::string::npos)
                        return false;
                    name_.resize(ix);
                    return true;
                }

                bool popRoot(std::string &root)
                {
                    auto ix = name_.find(Delimiter);
                    if (ix == std::string::npos)
                        return false;
                    root = name_.substr(0, ix);
                    name_ = name_.substr(ix+1);
                    return true;
                }
                bool popRoot()
                {
                    auto ix = name_.find(Delimiter);
                    if (ix == std::string::npos)
                        return false;
                    name_ = name_.substr(ix+1);
                    return true;
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
                Name name_;
        };
    }
}

#endif
