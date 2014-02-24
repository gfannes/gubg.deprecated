#ifndef HEADER_gubg_math_stat_Distribution_hpp_ALREADY_INCLUDED
#define HEADER_gubg_math_stat_Distribution_hpp_ALREADY_INCLUDED

namespace gubg
{
    template <typename Receiver, typename ValueType>
        class Distribution_crtp
        {
            public:
                typedef ValueType value_type;

                //double density(DomainT &value){return exp(logDensity(value));}
                //double logDensity(DomainT &value){return 0.0;}
                bool generate(value_type &value)
                {
                    return receiver_().distribution_generate(value);
                }
                template <typename Values>
                bool generate(Values &values)
                {
                    for (auto &v: values)
                    {
                        if (!generate(v))
                            return false;
                    }
                    return true;
                }

            private:
                Receiver &receiver_(){return static_cast<Receiver&>(*this);}
        };

#if 0
    // DomainC is the domain of the condition
    template <typename DomainT, typename DomainC>
        class ConditionalDistribution: public Distribution<DomainT>
    {
        public:
            typedef DomainT DomainType;
            typedef DomainC ConditionType;

            ConditionalDistribution(){}
            double density(DomainT &value, const DomainC &condition){return exp(logDensity(value,condition));}
            double logDensity(DomainT &value, const DomainC &condition){return 0.0;}
            bool generate(DomainT &value, const DomainC &condition)
            {
                cerr << "No generate functionality is implemented yet for this conditional distribution." << endl;
                return false;
            }
    };
#endif
}

#endif
