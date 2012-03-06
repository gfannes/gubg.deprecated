#ifndef gubg_coding_d9_io_hpp
#define gubg_coding_d9_io_hpp

#include "gubg/coding/d9/types.hpp"
#include "gubg/coding/d9/codes.hpp"

namespace gubg
{
    namespace coding
    {
        namespace d9
        {
            using namespace std;

            class IInput
            {
                public:
                    virtual ReturnCode read(ubyte &) = 0;
            };

            class FromString
            {
                public:
                    FromString(const string &str):
                        str_(str), ix(0), nr(str_.size()){}

                    bool get(int &v)
                    {
                        if (ix >= nr)
                            return false;
                        v = str_[ix++];
                        return true;
                    }

                private:
                    const string &str_;
                    size_t ix;
                    const size_t nr;
            };

            class FromStream
            {
                public:
                    FromStream(istream &str):
                        stream_(str){}

                    bool get(int &v)
                    {
                        if (!stream_.good())
                            return false;
                        v = stream_.get();
                        return v != EOF;
                    }

                private:
                    istream &stream_;
            };

            class FromByte
            {
                public:
                    template <typename Byte>
                    FromByte(Byte b):
                        v_(b){}

                    bool get(int &v)
                    {
                        if (v_ == EOF)
                            return false;
                        v = v_;
                        v_ = EOF;
                        return true;
                    }

                private:
                    int v_;
            };

            template <typename SourcePolicy>
                class Input: public IInput, private SourcePolicy
                {
                    public:
                        template <typename Source>
                        Input(Source &source):
                            SourcePolicy(source){}
                        template <typename Source>
                        Input(const Source &source):
                            SourcePolicy(source){}

                        virtual ReturnCode read(ubyte &b)
                        {
                            int v;
                            if (!SourcePolicy::get(v))
                                return ReturnCode::SourceIsEmpty;
                            b = 0xff&v;
                            return ReturnCode::OK;
                        }
                };
        }
    }
}

#endif
