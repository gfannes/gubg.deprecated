#ifndef gubg_string_hpp
#define gubg_string_hpp

namespace gubg
{
    namespace string_algo
    {
        template <typename String>
            size_t nrLeading(const char ch, const String &str)
            {
                for (auto it = str.begin(); it != str.end(); ++it)
                    if (ch != *it)
                        return it - str.begin();
                return str.size();
            }
        template <typename String>
            size_t nrTrailing(const char ch, const String &str)
            {
                for (auto it = str.rbegin(); it != str.rend(); ++it)
                    if (ch != *it)
                        return it - str.rbegin();
                //If all characters are == ch, we call them "leading", making sure you can't count them double
                return 0;
            }
    }
}

#endif
