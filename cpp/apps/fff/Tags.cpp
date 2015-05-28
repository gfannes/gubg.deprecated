#include "fff/Tags.hpp"

namespace fff { 

    Tag::Tags Tag::tags_;

    const Tag cache_dir{"cache_dir", "Directory to store the cached object, library and executable files"};
    const Tag cpp_define{"c++.define", "C++ macro definition"};
    const Tag cpp_executable{"c++.executable", "Name of executable produced by the linker"};
    const Tag cpp_flag{"c++.flag", "C++ compiler flag"};
    const Tag cpp_force_include{"c++.force_include", "C++ header to be included before all other includes"};
    const Tag cpp_header{"c++.header", "Full path to a C++ header file"};
    const Tag cpp_include{"c++.include", "Content of a #include statement"};
    const Tag cpp_include_path{"c++.include_path", "Header search path to be used by the compiler"};
    const Tag cpp_library{"c++.library", "Library to be linked against, without the lib prefix and file extension"};
    const Tag cpp_library_path{"c++.library_path", "Library search path to be used by the linker"};
    const Tag cpp_object{"c++.object", "Full path to compiled object file of the parent\'s C++ source"};
    const Tag cpp_shared_object{"c++.shared_object", "Name of shared object or dll produced by the linker"};
    const Tag cpp_source{"c++.source", "Full path to a C++ source file"};
    const Tag cpp_source_ref{"c++.source_ref", "Partial path to a C++ source file which needs to be resolved into a full path"};
    const Tag cpp_tree{"c++.tree", "Directory tree to be used for resolving C++ headers and source files. The last directory part needs to be specified in #include statements."};
    const Tag cpp_utree{"c++.utree", "Directory tree to be used for resolving C++ headers and source files"};
    const Tag c_header{"c.header", "Full path to a C header file"};
    const Tag c_object{"c.object", "Full path to compiled object file of the parent\'s C source"};
    const Tag c_source{"c.source", "Full path to a C source file"};
    const Tag c_source_ref{"c.source_ref", "Partial path to a C source file which needs to be resolved into a full path"};
    const Tag hash_tag{"hash.tag", "Tag for which an MD5 checksum on the item\'s file content should be computed"};
    const Tag hash_md5sum{"hash.md5sum", "MD5 checksum value of the parent\'s file content"};
    const Tag ig_ext{"ig.ext", "Extension to be handled by the include guard fixer"};
    const Tag ig_fix{"ig.fix", "When set to true, the include guard fixer will actually perform the fixes"};
    const Tag start{"start", "Startup argument given via the CLI"};

} 
