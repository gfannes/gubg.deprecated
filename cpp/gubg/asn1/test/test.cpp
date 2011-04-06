#include "asn1/Decoder.hpp"
#include "asn1/Encoder.hpp"
#include "Testing.hpp"
#include <iostream>
using namespace gubg;
using namespace std;

//#define L_ENABLE_DEBUG
#include "debug.hpp"

int main(int argc, char **argv)
{
    TEST_TAG(asn1);
    {
        TEST_TAG(encoder);
        asn1::Encoder encoder;
        {
            TEST_TAG(integer);
            encoder.clear();
            encoder.append(0);
            TEST_EQ(string({0x02, 0x01, 0}), encoder.encode());

            encoder.clear();
            encoder.append(123);
            TEST_EQ(string({0x02, 0x01, 123}), encoder.encode());

            encoder.clear();
            encoder.append(0x1234);
            TEST_EQ(string({0x02, 0x02, 0x12, 0x34}), encoder.encode());
        }

        {
            TEST_TAG(string);
            const asn1::Octets ia5({0x16, 0x05, 0x31, 0x32, 0x33, 0x34, 0x35});
            const asn1::Octets octet({0x04, 0x05, 0x31, 0x32, 0x33, 0x34, 0x35});
#define STR "12345"

            {
                TEST_TAG(inline_string);
                encoder.clear();
                encoder.append(STR);
                TEST_EQ(ia5, encoder.encode());

                encoder.clear();
                encoder.append(STR, asn1::StringType::IA5);
                TEST_EQ(ia5, encoder.encode());

                encoder.clear();
                encoder.append(STR, asn1::StringType::Octet);
                TEST_EQ(octet, encoder.encode());
            }

            {
                TEST_TAG(c_string);
                const char *cptr = STR;
                encoder.clear();
                encoder.append(cptr);
                TEST_EQ(ia5, encoder.encode());

                encoder.clear();
                encoder.append(cptr, asn1::StringType::IA5);
                TEST_EQ(ia5, encoder.encode());

                encoder.clear();
                encoder.append(cptr, asn1::StringType::Octet);
                TEST_EQ(octet, encoder.encode());
            }

            {
                TEST_TAG(std_string);
                const string str(STR);
                encoder.clear();
                encoder.append(str);
                TEST_EQ(ia5, encoder.encode());

                encoder.clear();
                encoder.append(str, asn1::StringType::IA5);
                TEST_EQ(ia5, encoder.encode());

                encoder.clear();
                encoder.append(str, asn1::StringType::Octet);
                TEST_EQ(octet, encoder.encode());
            }
        }

        {
            TEST_TAG(vector);
            encoder.clear();
            encoder.append(vector<int>{1, 2});
            TEST_EQ(string({0x30, 0x06,   0x02,0x01,1, 0x02,0x01,2}), encoder.encode());

            encoder.clear();
            encoder.append(vector<string>{"1", "2"});
            TEST_EQ(string({0x30, 0x06,   0x16,0x01,0x31, 0x16,0x01,0x32}), encoder.encode());

            encoder.clear();
            encoder.append(vector<const char *>{"1", "2"});
            TEST_EQ(string({0x30, 0x06,   0x16,0x01,0x31, 0x16,0x01,0x32}), encoder.encode());

            encoder.clear();
            encoder.append(vector<vector<int>>{{1, 2}, {3}});
            TEST_EQ(string({0x30,0x0d,   0x30,0x06, 0x02,0x01,1, 0x02,0x01,2,  0x30,0x03, 0x02,0x01,3}), encoder.encode());
        }

        {
            TEST_TAG(general);

            {
                TEST_TAG(simple);
                struct S
                {
                    S(int ii, const string &ss):i(ii), str(ss){}

                    void serialize(asn1::Encoder &encoder) const
                    {
                        encoder.append(i);
                        encoder.append(str);
                    }

                    int i;
                    string str;
                };
                S s(123, "456");

                encoder.clear();
                encoder.append(s);
                const string wanted({0x02,0x01,123,    0x16,0x03,0x34,0x35,0x36});
                TEST_EQ(wanted, encoder.encode());
            }

            {
                TEST_TAG(constructed);
                struct S
                {
                    S(int ii, const string &ss):i(ii), str(ss){}

                    void serialize(asn1::Encoder &encoder) const
                    {
                        asn1::Encoder subencoder;
                        subencoder.append(i);
                        subencoder.append(str);
                        encoder.append(subencoder, asn1::StructuredType::Sequence);
                    }

                    int i;
                    string str;
                };
                S s(123, "456");

                encoder.clear();
                encoder.append(s);
                const asn1::Octets wanted({0x30,0x08,   0x02,0x01,123,    0x16,0x03,0x34,0x35,0x36});
                TEST_EQ(wanted, encoder.encode());
            }
        }
    }

    {
        TEST_TAG(decoder);
        asn1::Decoder decoder;
        {
            TEST_TAG(integer);
            int i;
            {
                TEST_TAG(success);

                //Read 0
                decoder.reset(asn1::Octets({0x02, 0x00}));
                TEST_TRUE(decoder.extract(i));
                TEST_EQ(0, i);

                //Read positive, single-byte int
                decoder.reset(asn1::Octets({0x02, 0x01, 34}));
                TEST_TRUE(decoder.extract(i));
                TEST_EQ(34, i);

                //Read negative single-byte int
                decoder.reset(asn1::Octets({0x02, 0x01, 0x80}));
                TEST_TRUE(decoder.extract(i));
                TEST_EQ(-128, i);
                decoder.reset(asn1::Octets({0x02, 0x01, 0xf6}));
                TEST_TRUE(decoder.extract(i));
                TEST_EQ(-10, i);

                //Read positive, 2-byte int
                decoder.reset(asn1::Octets({0x02, 0x02, 0x00, 0xea}));
                TEST_TRUE(decoder.extract(i));
                TEST_EQ(234, i);

                //Read positive, 4-byte int
                decoder.reset(asn1::Octets({0x02, 0x04, 0x12, 0x34, 0x56, 0x78}));
                TEST_TRUE(decoder.extract(i));
                TEST_EQ(0x12345678, i);

                //Read 4-byte integer, negative
                decoder.reset(asn1::Octets({0x02, 0x02, 0x80, 0x00}));
                TEST_TRUE(decoder.extract(i));
                TEST_EQ(-32768, i);

                //Read 2 4-byte integers
                decoder.reset(asn1::Octets({0x02, 0x04, 0x12, 0x34, 0x56, 0x78,    0x02, 0x04, 0x12, 0x34, 0x56, 0x79}));
                TEST_TRUE(decoder.extract(i));
                TEST_EQ(0x12345678, i);
                TEST_TRUE(decoder.extract(i));
                TEST_EQ(0x12345679, i);
            }
            {
                TEST_TAG(failure);

                //No length
                decoder.reset(asn1::Octets({0x02}));
                TEST_FALSE(decoder.extract(i));

                //Not enough content
                decoder.reset(asn1::Octets({0x02, 0x01}));
                TEST_FALSE(decoder.extract(i));

                //Not an integer
                decoder.reset(asn1::Octets({0x03, 0x01, 34}));
                TEST_FALSE(decoder.extract(i));

                //Integer to long to fit in 4 bytes
                decoder.reset(asn1::Octets({0x02, 0x05, 1, 2, 3, 4, 5}));
                TEST_FALSE(decoder.extract(i));
            }
        }

        {
            TEST_TAG(string);
            string str;
            {
                TEST_TAG(success);

                //Read empty octet string
                decoder.reset(asn1::Octets({0x04, 0x00}));
                TEST_TRUE(decoder.extract(str));
                TEST_EQ("", str);

                //Read 5-byte octet string
                decoder.reset(asn1::Octets({0x04, 0x05, 0x31, 0x32, 0x33, 0x34, 0x35}));
                TEST_TRUE(decoder.extract(str));
                TEST_EQ("12345", str);

                //Read 5-byte IA5String
                decoder.reset(asn1::Octets({0x16, 0x05, 0x31, 0x32, 0x33, 0x34, 0x35}));
                TEST_TRUE(decoder.extract(str));
                TEST_EQ("12345", str);

                //Read 5-byte zero-filled octet string
                decoder.reset(asn1::Octets({0x04, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00}));
                TEST_TRUE(decoder.extract(str));
                string wanted = {0, 0, 0, 0, 0};
                TEST_EQ(wanted, str);

                //Read 2 octet strings
                decoder.reset(asn1::Octets({0x04, 0x05, 0x31, 0x32, 0x33, 0x34, 0x35,         0x04, 0x04, 0x34, 0x33, 0x32, 0x31}));
                TEST_TRUE(decoder.extract(str));
                TEST_EQ("12345", str);
                TEST_TRUE(decoder.extract(str));
                TEST_EQ("4321", str);
            }
            {
                TEST_TAG(failure);

                //Not an octet string nor IA5String
                decoder.reset(asn1::Octets({0x02, 0x01, 34}));
                TEST_FALSE(decoder.extract(str));
            }
        }

        {
            TEST_TAG(sequence_simple);
            vector<int> vec;

            {
                TEST_TAG(success);

                //Read sequence of 3 integers
                decoder.reset(asn1::Octets({0x30, 0x09,   0x02, 0x01, 1,   0x02, 0x01, 2,   0x02, 0x01, 3}));
                TEST_TRUE(decoder.extract(vec));
                vector<int> wanted = {1, 2, 3};
                TEST_EQ(wanted, vec);
            }
            {
                TEST_TAG(failure);

                //Not all integers
                decoder.reset(asn1::Octets({0x30, 0x09,   0x02, 0x01, 1,   0x02, 0x01, 2,   0x04, 0x01, 3}));
                TEST_FALSE(decoder.extract(vec));
            }
        }

        {
            TEST_TAG(subdecoder);

            //A sequence of 2 int (1, 2), followed by another int (99)
            const asn1::Octets der = {0x30, 0x06,   0x02, 0x01, 1,   0x02, 0x01, 2,         0x02, 0x01, 99};
            int i;
            {
                TEST_TAG(success);

                decoder.reset(der);
                {
                    //We read the first 2 ints using a subdecoder
                    asn1::Decoder subdecoder;
                    TEST_TRUE(decoder.extract(subdecoder));
                    bool ok = true;
                    TEST_TRUE(ok &= subdecoder.extract(i));
                    TEST_EQ(1, i);
                    TEST_TRUE(ok &= subdecoder.extract(i));
                    TEST_EQ(2, i);
                    TEST_TRUE(ok &= subdecoder.empty());
                    subdecoder.indicateDecodingOK(ok);
                }
                //We read the last int using the toplevel decoder
                TEST_TRUE(decoder.extract(i));
                TEST_EQ(99, i);
                TEST_TRUE(decoder.empty());
            }
            {
                TEST_TAG(failure);

                //We do the same as above, but try to make as many mistakes as possible
                decoder.reset(der);
                string str;
                {
                    //We read the first 2 ints using a subdecoder
                    asn1::Decoder subdecoder;
                    TEST_FALSE(decoder.extract(i));
                    TEST_TRUE(decoder.extract(subdecoder));
                    bool ok = true;
                    TEST_FALSE(subdecoder.empty());
                    TEST_FALSE(subdecoder.extract(str));
                    TEST_TRUE(ok &= subdecoder.extract(i));
                    TEST_FALSE(subdecoder.empty());
                    TEST_EQ(1, i);
                    TEST_FALSE(subdecoder.extract(str));
                    TEST_TRUE(ok &= subdecoder.extract(i));
                    TEST_EQ(2, i);
                    TEST_FALSE(subdecoder.extract(str));
                    TEST_FALSE(subdecoder.extract(i));
                    TEST_TRUE(ok &= subdecoder.empty());
                    TEST_THROW(asn1::Decoder::Blocked, decoder.extract(i));
                    subdecoder.indicateDecodingOK(ok);
                }
                //We read the last int using the toplevel decoder
                TEST_FALSE(decoder.extract(str));
                TEST_FALSE(decoder.empty());
                TEST_TRUE(decoder.extract(i));
                TEST_EQ(99, i);
                TEST_TRUE(decoder.empty());
            }
        }

        {
            TEST_TAG(general);

            struct S
            {
                int i;
                string str;

                bool operator==(const S &rhs) const
                {
                    return rhs.i == i && rhs.str == str;
                }

                private:
                friend class asn1::Decoder;
                bool construct(asn1::Decoder &decoder)
                {
                    return decoder.extract(i) && decoder.extract(str);
                }
            };
            S s;
            vector<S> vec;

            {
                TEST_TAG(success_single);

                //Read a single S from an integer and a string
                decoder.reset(asn1::Octets({0x02, 0x01, 34,    0x04, 0x05, 0x31, 0x32, 0x33, 0x34, 0x35}));
                TEST_TRUE(decoder.extract(s));
                TEST_EQ(34, s.i);
                TEST_EQ("12345", s.str);
            }
            {
                TEST_TAG(success_sequence);

                //Read a vector of S from a sequence of alternating integers and strings
                decoder.reset(asn1::Octets({0x30, 0x0c,   0x02, 0x01, 1,  0x04, 0x01, 0x31,   0x02, 0x01, 2,  0x04, 0x01, 0x32}));
                TEST_TRUE(decoder.extract(vec));
                vector<S> wanted = {{1, "1"}, {2, "2"}};
                TEST_EQ(wanted, vec);
            }
            {
                TEST_TAG(failure);

                //Cannot construct S from 2 integers
                decoder.reset(asn1::Octets({0x02, 0x01, 34,    0x02, 0x01, 35}));
                TEST_FALSE(decoder.extract(s));
                //Check that decoder remained unchanged and that we still can read the 2 integer directly
                int i;
                TEST_TRUE(decoder.extract(i));
                TEST_EQ(34, i);
                TEST_TRUE(decoder.extract(i));
                TEST_EQ(35, i);
            }
        }
    }

    {
        TEST_TAG(combined);
        asn1::Encoder encoder;
        asn1::Decoder decoder;

        struct S
        {
            bool operator==(const S &rhs) const
            {
                return rhs.i == i && rhs.str == str;
            }

            //Support for ASN.1 encoding and decoding
            void serialize(asn1::Encoder &encoder) const
            {
                asn1::Encoder subencoder;
                subencoder.append(i);
                subencoder.append(str);
                encoder.append(subencoder, asn1::StructuredType::Sequence);
            }
            bool construct(asn1::Decoder &decoder)
            {
                TEST_TAG(construct);
                asn1::Decoder subdecoder;
                bool ok = true;
                TEST_TRUE(ok &= decoder.extract(subdecoder));
                TEST_TRUE(ok &= subdecoder.extract(i));
                TEST_TRUE(ok &= subdecoder.extract(str));
                subdecoder.indicateDecodingOK(ok);
                return ok;
            }

            int i;
            string str;
        };
        const vector<S> orig = {{123, "123"}, {234, "234"}, {345, "345"}};
        vector<S> dup;

        encoder.append(orig);
        const auto encoded = encoder.encode();
        cout << toHex(encoded) << endl;
        decoder.reset(encoded);
        TEST_TRUE(decoder.extract(dup));
        TEST_EQ(orig, dup);
    }

    TEST_REPORT_TYPE(Full);
    return 0;
}
