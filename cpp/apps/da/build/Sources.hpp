#ifndef HEADER_da_build_Sources_hpp_ALREADY_INCLUDED
#define HEADER_da_build_Sources_hpp_ALREADY_INCLUDED

#include "da/build/Source.hpp"
#include "gubg/Verbose.hpp"
#include <vector>

namespace da
{
    class Sources: gubg::Verbose<false>
    {
        public:
            void clear() {sources_.clear();}

            Source::Ptr get(Source::File) const;

            Source::Ptr add(Source::File);

            typedef std::vector<Source::Ptr> Sources_;
            typedef Sources_::const_iterator const_iterator;
            const_iterator begin() const {return sources_.begin();}
            const_iterator end() const {return sources_.end();}

        private:
            Sources_ sources_;
    };
}

#endif
