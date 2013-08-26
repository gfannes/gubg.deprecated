#ifndef aoeuaoeu
#define aoeuaoeu

#include "gubg/msgpack/Write.hpp"
#include <vector>

#define GUBG_MODULE "Serializer"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace msgpack
    {
        typedef long TypeId;
        typedef long AttrId;

        template <typename Buffer>
            class Serializer
            {
                public:
                    void clear()
                    {
                        buffer_.clear();
                        attrInfos_.clear();
                    }
                    ReturnCode swap(Buffer &buffer)
                    {
                        MSS_BEGIN(ReturnCode);
                        MSS(attrInfos_.empty());
                        buffer_.swap(buffer);
                        MSS_END();
                    }

                    template <typename T>
                        ReturnCode serialize(const T &t)
                        {
                            MSS_BEGIN(ReturnCode);
                            MSS(t.msgpack_serialize(*this));
                            MSS_END();
                        }
                    ReturnCode serialize(long v) { return write(buffer_, v); }
                    ReturnCode serialize(int v) { return write(buffer_, v); }
                    ReturnCode serialize(const std::string &str) { return write(buffer_, str); }

                    ReturnCode writeIdAndAttrCnt(TypeId tid, size_t attr_cnt)
                    {
                        MSS_BEGIN(ReturnCode);
                        MSS(write(buffer_, attr_cnt + 1, MapTL_tag()));
                        if (attr_cnt > 0)
                        {
                            AttrInfo ai;
                            ai.nr = attr_cnt;
                            ai.ix = 0;
                            attrInfos_.push_back(ai);
                        }
                        MSS(write(buffer_, Nil_tag()));
                        MSS(write(buffer_, tid));
                        MSS_END();
                    }
                    template <typename T>
                        ReturnCode writeAttribute(AttrId aid, const T &t)
                        {
                            MSS_BEGIN(ReturnCode);
                            MSS(write(buffer_, aid));
                            MSS(serialize(t));
                            auto &ai = attrInfos_.back();
                            ++ai.ix;
                            if (ai.ix == ai.nr)
                                attrInfos_.pop_back();
                            MSS_END();
                        }

                private:
                    Buffer buffer_;
                    struct AttrInfo
                    {
                        size_t nr;
                        size_t ix;
                    };
                    typedef std::vector<AttrInfo> AttrInfos;
                    AttrInfos attrInfos_;
            };
    }
}
#include "gubg/log/end.hpp"

#endif