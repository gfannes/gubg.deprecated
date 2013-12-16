#ifndef HEADER_rinle_model_Location_hpp_ALREADY_INCLUDED
#define HEADER_rinle_model_Location_hpp_ALREADY_INCLUDED

#include "rinle/Types.hpp"
#include <string>
#include <ostream>
#include <cassert>

#define GUBG_MODULE_ "Location"
#include "gubg/log/begin.hpp"
namespace rinle { namespace model {

    struct Location
    {
        Lines &lines;
        size_t row;
        size_t col;
        Location(Lines &l): lines(l), row(0), col(0) {}
        Location begin() const
        {
            return Location(lines);
        }
        Location end() const
        {
            Location ret(lines);
            ret.row = lines.size();
            return ret;
        }
        bool operator==(const Location &rhs) const
        {
            assert(&lines == &rhs.lines);
            return std::make_pair(row, col) == std::make_pair(rhs.row, rhs.col);
        }
        bool operator!=(const Location &rhs) const
        {
            assert(&lines == &rhs.lines);
            return std::make_pair(row, col) != std::make_pair(rhs.row, rhs.col);
        }
        bool operator<(const Location &rhs) const
        {
            assert(&lines == &rhs.lines);
            return std::make_pair(row, col) < std::make_pair(rhs.row, rhs.col);
        }
        bool operator<=(const Location &rhs) const
        {
            assert(&lines == &rhs.lines);
            return std::make_pair(row, col) <= std::make_pair(rhs.row, rhs.col);
        }
        Token *operator->() { return &lines[row][col]; }
        Location &operator++()
        {
            while (row < lines.size())
            {
                if (col+1 < lines[row].size())
                {
                    ++col;
                    break;
                }
                else
                {
                    ++row;
                    col = 0;
                    if (row < lines.size() && col < lines[row].size())
                        break;
                }
            }
            return *this;
        }
        Location &operator--()
        {
            if (row == 0 && col == 0)
                return *this;

            //Decrease row until we are within lines
            while (row >= lines.size())
            {
                if (row == 0)
                    return *this;
                --row;
                if (row < lines.size())
                    col = lines[row].size();
            }
            while (row < lines.size())
            {
                if (col > 0)
                {
                    //We found the previous position
                    --col;
                    return *this;
                }
                if (row == 0)
                    return *this;
                --row;
                col = lines[row].size();
            }
            return *this;
        }
        Location &operator+=(int n)
        {
            if (n == 0) {}
            else if (n > 0)
            {
                for (int i = 0; i < n; ++i)
                    operator++();
            }
            else if (n < 0)
            {
                for (int i = 0; i < -n; ++i)
                    operator--();
            }
            return *this;
        }
    };
    inline std::ostream &operator<<(std::ostream &os, const Location &l)
    {
        return os << "Location(" << l.row << ", " << l.col << ")";
    }

} }
#include "gubg/log/end.hpp"

#endif
