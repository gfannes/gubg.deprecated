#ifndef HEADER_gubg_file_Filesystem_hpp_ALREADY_INCLUDED
#define HEADER_gubg_file_Filesystem_hpp_ALREADY_INCLUDED

#include "gubg/file/File.hpp"
#include "gubg/file/Codes.hpp"
#include "gubg/SmartRange.hpp"
#include <string>
#include <vector>

namespace gubg
{
    namespace file
    {
        //Returns the size of File
        ReturnCode size(size_t &, const File &);

        //Reads the content of a File
        ReturnCode read(std::string &content, const File &);
        ReturnCode read(SmartRange<std::string> &range, const File &);
        ReturnCode read(std::vector<File> &, const File &);

        //Write a file
        ReturnCode write(const std::string &content, const File &);

        //Remove a file
        ReturnCode remove(const File &);

        //Copy a file
        ReturnCode copy(const File &from, const File &to);

        ReturnCode determineType(File &file);

        bool exists(const File &);
        bool isRegular(const File &);
        bool isDirectory(const File &);

        ReturnCode getcwd(File &);
        File getcwd();

        //Calls recursor_discoveredFile(file) to indicate a new file was discovered
        //* OK: continue
        //* Skip: skip this file
        //* Stop: stop recursing
        template <typename Receiver>
            class Recursor_crtp
            {
                public:
                    ReturnCode operator()(const File &file)
                    {
                        MSS_BEGIN(ReturnCode);
                        File ff(file);
                        MSS(determineType(ff));
                        MSS(recurse_(ff));
                        MSS_END();
                    }

                private:
                    Receiver &receiver_(){return static_cast<Receiver&>(*this);}
                    ReturnCode recurse_(const File &file)
                    {
                        MSS_BEGIN(ReturnCode);
                        typedef decltype(receiver_().recursor_discoveredFile(file)) RC;
                        switch (auto rc = receiver_().recursor_discoveredFile(file))
                        {
                            case RC::OK:
                                {
                                    std::vector<File> files;
                                    if (mss::isOK(read(files, file)))
                                        for (auto ff: files)
                                        {
                                            switch (auto rc2 = recurse_(ff))
                                            {
                                                case ReturnCode::Stop: MSS_Q(rc2); break;
                                                default:               MSS(rc2); break;
                                            }
                                        }
                                }
                                break;
                            case RC::Skip:          break;
                            case RC::Stop: MSS_Q(ReturnCode::Stop); break;
                            default:       MSS(rc); break;
                        }
                        MSS_END();
                    }
            };
    }
}

#endif
