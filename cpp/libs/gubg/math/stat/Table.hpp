#ifndef HEADER_gubg_math_stat_Table_hpp_ALREADY_INCLUDED
#define HEADER_gubg_math_stat_Table_hpp_ALREADY_INCLUDED

#include "gubg/math/stat/Distribution.hpp"

#define GUBG_MODULE "Table"
#include "gubg/log/begin.hpp"
namespace gubg
{
    class Table: public Distribution_crtp<Table, size_t>
    {
        public:
            typedef double T;

            Table(size_t nr):
                table_(nr){}

        private:
            friend class Distribution_crtp<Table, T>;
            bool distribution_generate(size_t &ix)
            {
                return table_.generate(ix);
            }

            distribution::Table table_;
    };
}
#include "gubg/log/end.hpp"

#endif
