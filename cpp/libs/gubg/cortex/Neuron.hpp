#ifndef HEADER_gubg_cortex_Neuron_hpp_ALREADY_INCLUDED
#define HEADER_gubg_cortex_Neuron_hpp_ALREADY_INCLUDED

#include "gubg/cortex/ReturnCodes.hpp"
#include <vector>
#include <cmath>

#include <iostream>
#define LOG(m) std::cout << m << std::endl

namespace gubg
{
    namespace cortex
    {
        using namespace std;

        template <typename FF>
            struct TanhTransfer
            {
                    FF operator()(FF ff){return 1.0-2.0/(1.0+::exp(ff+ff));}
            };
        template <typename FF>
            struct LinearTransfer
            {
                    FF operator()(FF ff){return ff;}
            };

        template <typename FF>
            struct UseBias
            {
                UseBias():bias_(0){}
                FF value() const {return bias_;}
                void setBias(FF bias){bias_ = bias;}
                FF bias_;
            };
        template <typename FF>
            struct NoBias
            {
                FF value() const {return 0;}
            };

        template <typename FF>
            struct UseWeights
            {
                typedef vector<FF> Inputs;
                typedef vector<FF> Weights;
                ReturnCode compute(FF &res, const Inputs &inputs)
                {
                    MSS_BEGIN(ReturnCode);
                    res = 0;
                    MSS(inputs.size() == weights_.size(), InputSizeError);
                    for (size_t ix = 0; ix < inputs.size(); ++ix)
                        res += inputs[ix]*weights_[ix];
                    MSS_END();
                }
                void setWeights(const Weights &weights){weights_ = weights;}
                Weights weights_;
            };

        template <typename FF_t, template <typename F> class OutputP, template <typename F> class BiasP = UseBias, template <typename F> class InputP = UseWeights>
            class NeuronT: public OutputP<FF_t>, public BiasP<FF_t>, public InputP<FF_t>
            {
                public:
                    typedef FF_t FF;
                    typedef OutputP<FF> OutputPolicy;
                    typedef BiasP<FF> BiasPolicy;
                    typedef InputP<FF> InputPolicy;
                    typedef typename InputPolicy::Inputs Inputs;

                    ReturnCode compute(FF &output, const Inputs &inputs)
                    {
                        MSS_BEGIN(ReturnCode);
                        MSS(InputPolicy::compute(output, inputs));
                        output += BiasPolicy::value();
                        output = OutputPolicy::operator()(output);
                        MSS_END();
                    }
            };
    }
}

#endif