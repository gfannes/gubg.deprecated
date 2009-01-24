#include <iostream>

#include "mlp.hpp"
#include "vector.hpp"

using namespace std;

int main()
{
    Random::setSeed();

    vector<int> nrPerLayer;
    vector<double> input;
    vector<NeuronE> typePerLayer;

    cout << "*****************************************************************************" << endl;
    cout << "Tests for global output == vector" << endl;
    typedef MLP<vector<double> > MLPTV;
    Vector::set(nrPerLayer,4,5,2);
    Vector::set(typePerLayer,eNeuronTanh,eNeuronTanh,eNeuronSigmoid);
    MLPTV mlpV(2,nrPerLayer,typePerLayer);
    mlpV.setStructure(2,nrPerLayer,typePerLayer);
    Vector::set(input,-2.0,1.0);
    MLPTV::ParameterType *parV;
    parV=mlpV.generateParameter();
    cout << "MLPV parameters = \n" << *parV << endl;
    mlpV.setInput(input);
    mlpV.setParameter(*parV);
    cout << "Output == " << mlpV.output() << endl;
    MLPTV::ParameterDerivativeType parDerV;
    mlpV.computeDerivativeParameter(parDerV);
    cout << "derivative to param:\n" << parDerV << "" << endl;

    cout << "*****************************************************************************" << endl;
    cout << "Tests for global output == double" << endl;
    typedef MLP<double> MLPTD;
    Vector::set(nrPerLayer,4,5,1);
    Vector::set(typePerLayer,eNeuronTanh,eNeuronTanh,eNeuronSigmoid);
    MLPTD mlpD(2,nrPerLayer,typePerLayer);
    mlpD.setStructure(2,nrPerLayer,typePerLayer);
    Vector::set(input,-2.0,1.0);
    MLPTD::ParameterType *parD;
    parD=mlpD.generateParameter();
    cout << "MLPD parameters = \n" << *parD << endl;
    mlpD.setInput(input);
    mlpD.setParameter(*parD);
    cout << "Output == " << mlpD.output() << endl;
    MLPTD::ParameterDerivativeType parDerD;
    mlpD.computeDerivativeParameter(parDerD);
    cout << "derivative to param:\n" << parDerD << "" << endl;

    return 0;
}
