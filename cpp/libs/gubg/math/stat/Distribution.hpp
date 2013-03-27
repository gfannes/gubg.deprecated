#ifndef HEADER_gubg_math_distribution_distribution_hpp_ALREADY_INCLUDED
#define HEADER_gubg_math_distribution_distribution_hpp_ALREADY_INCLUDED

using namespace std;

namespace gubg
{
    template <typename Receiver, typename DomainT>
        class Distribution_crtp
        {
            public:
                typedef DomainT DomainType;

                double density(DomainT &value){return exp(logDensity(value));}
                double logDensity(DomainT &value){return 0.0;}
                bool draw(DomainT &value)
                {
                    cerr << "No draw functionality is implemented yet for this distribution." << endl;
                    return false;
                }
            private:
        };

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
            bool draw(DomainT &value, const DomainC &condition)
            {
                cerr << "No draw functionality is implemented yet for this conditional distribution." << endl;
                return false;
            }
    };
}

#endif
