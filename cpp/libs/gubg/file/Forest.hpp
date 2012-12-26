#ifndef HEADER_gubg_file_Forest_hpp_ALREADY_INCLUDED
#define HEADER_gubg_file_Forest_hpp_ALREADY_INCLUDED

#include "gubg/file/Codes.hpp"
#include "gubg/file/File.hpp"
#include <vector>
#include <memory>

namespace gubg
{
    namespace file
    {
        class Tree
        {
            public:
                typedef std::vector<std::string> Extensions;
                typedef std::vector<File> Files;

                typedef std::shared_ptr<Tree> Ptr;
                static Ptr create(const File &root, const Extensions &);

                ReturnCode add(const File &);

                const File &root() const {return root_;}
                const Files &files() const {return files_;}

            private:
                Tree(const File &root, const Extensions &);

                const File root_;
                const Extensions extensions_;
                Files files_;
        };

        class Forest
        {
            public:
                ReturnCode add(const File &root, const Tree::Extensions &);

                typedef std::vector<File> Files;
                Files allFiles() const;
                Files allRoots() const;

                ReturnCode resolve(File &resolved, const File &partial, const size_t overlap = 1) const;
                bool contains(const File &) const;

            private:
                std::vector<Tree::Ptr> trees_;
        };
    }
}

#endif
