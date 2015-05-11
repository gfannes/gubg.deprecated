#ifndef HEADER_gubg_db_KeyValue_hpp_ALREADY_INCLUDED
#define HEADER_gubg_db_KeyValue_hpp_ALREADY_INCLUDED

#include "gubg/db/Codes.hpp"
#include "gubg/file/File.hpp"
#include "gubg/file/Filesystem.hpp"
#include "gubg/hash/MD5.hpp"
#include <string>

#define GUBG_MODULE "KeyValue"
#include "gubg/log/begin.hpp"
namespace gubg { namespace db { 

    using Value = std::string;
    using Key = std::string;

    //Simple filename-based key-value database
    class KeyValue
    {
        public:
            KeyValue(gubg::file::File dir): dir_(dir)
            {
                file::mkdir(dir);
            }

            ReturnCode valid() const
            {
                MSS_BEGIN(ReturnCode);
                MSS(file::isDirectory(dir_));
                MSS_END();
            }

            ReturnCode get(Value &value, const Key &key)
            {
                MSS_BEGIN(ReturnCode);
                auto fn = dir_; fn << hash_(key); fn.setExtension("value");
                MSS(file::read(value, fn));
                MSS_END();
            }

            ReturnCode set(const Key &key, const Value &value)
            {
                MSS_BEGIN(ReturnCode);
                auto fn = dir_; fn << hash_(key);
                {
                    fn.setExtension("key");
                    MSS(file::write(key, fn));
                }
                {
                    fn.setExtension("value");
                    MSS(file::write(value, fn));
                }
                MSS_END();
            }

        private:
            static std::string hash_(const Key &key)
            {
                gubg::hash::md5::Stream md5;
                md5 << key;
                return md5.hash_hex();
            }
            gubg::file::File dir_;
    };

} } 
#include "gubg/log/end.hpp"

#endif
