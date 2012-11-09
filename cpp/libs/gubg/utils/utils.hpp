#ifndef HEADER_gubg_utils_utils_hpp_ALREADY_INCLUDED
#define HEADER_gubg_utils_utils_hpp_ALREADY_INCLUDED

#include <sstream>

using namespace std;

string indent(int level)
{
    ostringstream ostr;
    for (; level > 0 ; level--)
        ostr << "  ";
    return ostr.str();
}

namespace Utils
{   
//begin template
//R (1..10).each do |i|
//R   args=(1..i).collect{|j|"T v#{j}"}.join(',')
//R   ifs=(2..i).collect{|j|"if (v#{j} > m) m = v#{j};"}.join(" ")
//C    template<typename T>
//C    T max(#{args})
//C    {
//C      T m = v1;
//C      #{ifs}
//C      return m;
//C    }
//R end

    template<typename T>
    T max(T v1)
    {
      T m = v1;
      
      return m;
    }

    template<typename T>
    T max(T v1,T v2)
    {
      T m = v1;
      if (v2 > m) m = v2;
      return m;
    }

    template<typename T>
    T max(T v1,T v2,T v3)
    {
      T m = v1;
      if (v2 > m) m = v2; if (v3 > m) m = v3;
      return m;
    }

    template<typename T>
    T max(T v1,T v2,T v3,T v4)
    {
      T m = v1;
      if (v2 > m) m = v2; if (v3 > m) m = v3; if (v4 > m) m = v4;
      return m;
    }

    template<typename T>
    T max(T v1,T v2,T v3,T v4,T v5)
    {
      T m = v1;
      if (v2 > m) m = v2; if (v3 > m) m = v3; if (v4 > m) m = v4; if (v5 > m) m = v5;
      return m;
    }

    template<typename T>
    T max(T v1,T v2,T v3,T v4,T v5,T v6)
    {
      T m = v1;
      if (v2 > m) m = v2; if (v3 > m) m = v3; if (v4 > m) m = v4; if (v5 > m) m = v5; if (v6 > m) m = v6;
      return m;
    }

    template<typename T>
    T max(T v1,T v2,T v3,T v4,T v5,T v6,T v7)
    {
      T m = v1;
      if (v2 > m) m = v2; if (v3 > m) m = v3; if (v4 > m) m = v4; if (v5 > m) m = v5; if (v6 > m) m = v6; if (v7 > m) m = v7;
      return m;
    }

    template<typename T>
    T max(T v1,T v2,T v3,T v4,T v5,T v6,T v7,T v8)
    {
      T m = v1;
      if (v2 > m) m = v2; if (v3 > m) m = v3; if (v4 > m) m = v4; if (v5 > m) m = v5; if (v6 > m) m = v6; if (v7 > m) m = v7; if (v8 > m) m = v8;
      return m;
    }

    template<typename T>
    T max(T v1,T v2,T v3,T v4,T v5,T v6,T v7,T v8,T v9)
    {
      T m = v1;
      if (v2 > m) m = v2; if (v3 > m) m = v3; if (v4 > m) m = v4; if (v5 > m) m = v5; if (v6 > m) m = v6; if (v7 > m) m = v7; if (v8 > m) m = v8; if (v9 > m) m = v9;
      return m;
    }

    template<typename T>
    T max(T v1,T v2,T v3,T v4,T v5,T v6,T v7,T v8,T v9,T v10)
    {
      T m = v1;
      if (v2 > m) m = v2; if (v3 > m) m = v3; if (v4 > m) m = v4; if (v5 > m) m = v5; if (v6 > m) m = v6; if (v7 > m) m = v7; if (v8 > m) m = v8; if (v9 > m) m = v9; if (v10 > m) m = v10;
      return m;
    }

//end template
   
//begin template
//R (1..10).each do |i|
//R   args=(1..i).collect{|j|"T v#{j}"}.join(',')
//R   ifs=(2..i).collect{|j|"if (v#{j} < m) m = v#{j};"}.join(" ")
//C    template<typename T>
//C    T min(#{args})
//C    {
//C      T m = v1;
//C      #{ifs}
//C      return m;
//C    }
//R end

    template<typename T>
    T min(T v1)
    {
      T m = v1;
      
      return m;
    }

    template<typename T>
    T min(T v1,T v2)
    {
      T m = v1;
      if (v2 < m) m = v2;
      return m;
    }

    template<typename T>
    T min(T v1,T v2,T v3)
    {
      T m = v1;
      if (v2 < m) m = v2; if (v3 < m) m = v3;
      return m;
    }

    template<typename T>
    T min(T v1,T v2,T v3,T v4)
    {
      T m = v1;
      if (v2 < m) m = v2; if (v3 < m) m = v3; if (v4 < m) m = v4;
      return m;
    }

    template<typename T>
    T min(T v1,T v2,T v3,T v4,T v5)
    {
      T m = v1;
      if (v2 < m) m = v2; if (v3 < m) m = v3; if (v4 < m) m = v4; if (v5 < m) m = v5;
      return m;
    }

    template<typename T>
    T min(T v1,T v2,T v3,T v4,T v5,T v6)
    {
      T m = v1;
      if (v2 < m) m = v2; if (v3 < m) m = v3; if (v4 < m) m = v4; if (v5 < m) m = v5; if (v6 < m) m = v6;
      return m;
    }

    template<typename T>
    T min(T v1,T v2,T v3,T v4,T v5,T v6,T v7)
    {
      T m = v1;
      if (v2 < m) m = v2; if (v3 < m) m = v3; if (v4 < m) m = v4; if (v5 < m) m = v5; if (v6 < m) m = v6; if (v7 < m) m = v7;
      return m;
    }

    template<typename T>
    T min(T v1,T v2,T v3,T v4,T v5,T v6,T v7,T v8)
    {
      T m = v1;
      if (v2 < m) m = v2; if (v3 < m) m = v3; if (v4 < m) m = v4; if (v5 < m) m = v5; if (v6 < m) m = v6; if (v7 < m) m = v7; if (v8 < m) m = v8;
      return m;
    }

    template<typename T>
    T min(T v1,T v2,T v3,T v4,T v5,T v6,T v7,T v8,T v9)
    {
      T m = v1;
      if (v2 < m) m = v2; if (v3 < m) m = v3; if (v4 < m) m = v4; if (v5 < m) m = v5; if (v6 < m) m = v6; if (v7 < m) m = v7; if (v8 < m) m = v8; if (v9 < m) m = v9;
      return m;
    }

    template<typename T>
    T min(T v1,T v2,T v3,T v4,T v5,T v6,T v7,T v8,T v9,T v10)
    {
      T m = v1;
      if (v2 < m) m = v2; if (v3 < m) m = v3; if (v4 < m) m = v4; if (v5 < m) m = v5; if (v6 < m) m = v6; if (v7 < m) m = v7; if (v8 < m) m = v8; if (v9 < m) m = v9; if (v10 < m) m = v10;
      return m;
    }

//end template
};
#endif
