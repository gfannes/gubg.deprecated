#ifndef gubg_file_Filesystem_hpp
#define gubg_file_Filesystem_hpp

#include "gubg/file/File.hpp"
#include "gubg/file/Codes.hpp"
#include <string>
#include <vector>

namespace gubg
{
    namespace file
    {
        //Returns the size of File
        ReturnCode size(size_t &, const File &);

        //Returns the content of a File
        ReturnCode read(std::string &content, const File &);
        ReturnCode read(std::vector<File> &, const File &);

        template <typename Receiver>
            class Recursor_crtp
            {
                public:
                    void operator()(const File &file)
                    {
                        receiver_().recursor_discoveredFile(file);
                        std::vector<File> files;
                        if (mss::isOK(read(files, file)))
                            for (auto ff: files)
                                operator()(ff);
                    }

                private:
                    Receiver &receiver_(){return static_cast<Receiver&>(*this);}
            };
    }
}

#endif
