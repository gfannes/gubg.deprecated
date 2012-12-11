#ifndef gubg_hash_MD5_hpp
#define gubg_hash_MD5_hpp

#include <array>
#include <cstdint>

namespace gubg
{
    namespace hash
    {
        class MD5
        {
            public:
                typedef std::uint32_t Word;
                typedef std::array<Word, 4> Hash;

                MD5();

                void clear();

            private:
                void processBlock_(const Word *);

                Hash hash_;
        };
    }
}

#endif
