#include "gubg/coding/d9/types.hpp"
using namespace std;

namespace gubg
{
    namespace coding
    {
        namespace d9
        {
            string to_s(Format format)
            {
                switch (format)
                {
                    case Format::AsIs:   return "asis format"; break;
                    case Format::Block:  return "block format"; break;
                    case Format::Stream: return "stream format"; break;
                }
                return "unknown format";
            }
        }
    }
}
