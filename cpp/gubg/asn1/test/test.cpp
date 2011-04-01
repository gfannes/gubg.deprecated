#include "ASN1.hpp"
#include "Testing.hpp"
#include <iostream>
using namespace gubg;
using namespace std;

int main(int argc, char **argv)
{
    TEST_SUITE(asn1);
    asn1::Decoder decoder("");
    {
        TEST_CASE(integer);
        int i;
        {
            TEST_PART(success);

            //Read 0
            decoder.reset({0x02, 0x00});
            TEST_TRUE(decoder.decode(i));
            TEST_EQ(0, i);

            //Read 0x34, single byte
            decoder.reset({0x02, 0x01, 34});
            TEST_TRUE(decoder.decode(i));
            TEST_EQ(34, i);

            //Read -128, single byte but negative
            decoder.reset({0x02, 0x01, -128});
            TEST_TRUE(decoder.decode(i));
            TEST_EQ(-128, i);

            //Read 4-byte integer, positive
            decoder.reset({0x02, 0x04, 0x12, 0x34, 0x56, 0x78});
            TEST_TRUE(decoder.decode(i));
            TEST_EQ(0x12345678, i);

            //Read 4-byte integer, negative
            decoder.reset({0x02, 0x02, -128, 0x00});
            TEST_TRUE(decoder.decode(i));
            TEST_EQ(-32768, i);

            //Read 2 4-byte integers
            decoder.reset({0x02, 0x04, 0x12, 0x34, 0x56, 0x78,    0x02, 0x04, 0x12, 0x34, 0x56, 0x79});
            TEST_TRUE(decoder.decode(i));
            TEST_EQ(0x12345678, i);
            TEST_TRUE(decoder.decode(i));
            TEST_EQ(0x12345679, i);
        }
        {
            TEST_PART(failure);

            //No length
            decoder.reset({0x02});
            TEST_FALSE(decoder.decode(i));

            //Not enough content
            decoder.reset({0x02, 0x01});
            TEST_FALSE(decoder.decode(i));

            //Not an integer
            decoder.reset({0x03, 0x01, 34});
            TEST_FALSE(decoder.decode(i));

            //Integer to long to fit in 4 bytes
            decoder.reset({0x02, 0x05, 1, 2, 3, 4, 5});
            TEST_FALSE(decoder.decode(i));
        }
    }

    {
        TEST_CASE(string);
        string str;
        {
            TEST_PART(success);

            //Read empty octet string
            decoder.reset({0x04, 0x00});
            TEST_TRUE(decoder.decode(str));
            TEST_EQ("", str);

            //Read 5-byte octet string
            decoder.reset({0x04, 0x05, 0x31, 0x32, 0x33, 0x34, 0x35});
            TEST_TRUE(decoder.decode(str));
            TEST_EQ("12345", str);

            //Read 5-byte IA5String
            decoder.reset({0x16, 0x05, 0x31, 0x32, 0x33, 0x34, 0x35});
            TEST_TRUE(decoder.decode(str));
            TEST_EQ("12345", str);

            //Read 5-byte zero-filled octet string
            decoder.reset({0x04, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00});
            TEST_TRUE(decoder.decode(str));
            string wanted = {0, 0, 0, 0, 0};
            TEST_EQ(wanted, str);

            //Read 2 octet strings
            decoder.reset({0x04, 0x05, 0x31, 0x32, 0x33, 0x34, 0x35,         0x04, 0x04, 0x34, 0x33, 0x32, 0x31});
            TEST_TRUE(decoder.decode(str));
            TEST_EQ("12345", str);
            TEST_TRUE(decoder.decode(str));
            TEST_EQ("4321", str);
        }
        {
            TEST_PART(failure);

            //Not an octet string nor IA5String
            decoder.reset({0x02, 0x01, 34});
            TEST_FALSE(decoder.decode(str));
        }
    }

    {
        TEST_CASE(general);

        struct S
        {
            int i;
            string str;

            private:
            friend class asn1::Decoder;
            bool construct(asn1::Decoder &decoder)
            {
                return decoder.decode(i) && decoder.decode(str);
            }
        };
        S s;

        {
            TEST_PART(success);

            decoder.reset({0x02, 0x01, 34,    0x04, 0x05, 0x31, 0x32, 0x33, 0x34, 0x35});
            TEST_TRUE(decoder.decode(s));
            TEST_EQ(34, s.i);
            TEST_EQ("12345", s.str);
        }
        {
            TEST_PART(failure);

            //Cannot construct S from 2 integers
            decoder.reset({0x02, 0x01, 34,    0x02, 0x01, 35});
            TEST_FALSE(decoder.decode(s));
            //Check that decoder remained unchanged and that we still can read the 2 integer directly
            int i;
            TEST_TRUE(decoder.decode(i));
            TEST_EQ(34, i);
            TEST_TRUE(decoder.decode(i));
            TEST_EQ(35, i);
        }
    }
    return 0;
}
