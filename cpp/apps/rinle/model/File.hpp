#ifndef HEADER_rinle_model_File_hpp_ALREADY_INCLUDED
#define HEADER_rinle_model_File_hpp_ALREADY_INCLUDED

#include "gubg/file/File.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/parse/Line.hpp"
#include <string>

namespace rinle
{
    namespace model
    {
        class File
        {
            public:
                File(const std::string &path): path_(path) {load_();}

                gubg::line::Lines lines() const
                {
                    return gubg::line::split(content_);
                }

            private:
                void load_()
                {
                    gubg::file::read(content_, path_);
                }

                gubg::file::File path_;
                std::string content_;
        };
    }
}

#endif
