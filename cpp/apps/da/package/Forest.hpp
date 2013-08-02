#ifndef HEADER_da_package_Forest_hpp_ALREADY_INCLUDED
#define HEADER_da_package_Forest_hpp_ALREADY_INCLUDED

#include "da/package/Package.hpp"
#include "gubg/file/File.hpp"
#include "gubg/file/Forest.hpp"

namespace da
{
    namespace package
    {
        class Forest: public Package
        {
            public:
				typedef std::shared_ptr<Forest> Ptr;
                static Ptr create(){return Ptr(new Forest);}

                //Package API
                virtual std::string name() const {return "forest";}
                virtual bool exists() const;
                virtual ReturnCode resolveHeader(File &resolvedHeader, SourceFiles &sisterFiles, const File &partial);

				void addTree(const gubg::file::File &tree);

            private:
                Forest(){}

                gubg::file::Forest forest_;
        };
    }
}

#endif
