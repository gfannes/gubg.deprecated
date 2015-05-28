#include "fff/Tags.hpp"

namespace fff { 

    Tag::Tags Tag::tags_;

    const Tag cpp_define{"c++.define", "C++ macro definition"};
    const Tag cpp_header{"c++.header", "Full path to a C++ header file"};
    const Tag cpp_source{"c++.source", "Full path to a C++ source file"};
    const Tag cpp_source_ref{"c++.source_ref", "Partial path to a C++ source file which needs to be resolved into a full path"};
    const Tag c_source{"c.source", "Full path to a C source file"};
    const Tag c_source_ref{"c.source_ref", "Partial path to a C source file which needs to be resolved into a full path"};
    const Tag hash_tag{"hash.tag", "Tag for which an MD5 checksum on the item\'s file content should be computed"};

} 
