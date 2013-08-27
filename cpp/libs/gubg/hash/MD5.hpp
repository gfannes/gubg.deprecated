#ifndef HEADER_gubg_hash_MD5_hpp_ALREADY_INCLUDED
#define HEADER_gubg_hash_MD5_hpp_ALREADY_INCLUDED

#include <array>
#include <string>
#include <cstdint>

namespace gubg
{
    namespace hash
    {
        class MD5
        {
            public:
                MD5();

                void clear();

                MD5 &operator<<(const std::string &);

                typedef std::array<uint8_t, 16> Hash;
                Hash hash() const;
                std::string hash_hex() const;

                static std::string to_hex(const Hash &);

            private:
                typedef std::array<uint32_t, 4> HashWords;
                HashWords hash_;
                std::string remainder_;
				//This should stay 64 bit and little endian, its content is copied as-is
                typedef std::uint64_t Length;
                Length length_;
        };
    }
}

#endif
