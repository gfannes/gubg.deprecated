#include <iostream>
#include <vector>

#include "posteriorLogProb.hpp"
#include "neuron.hpp"
#include "data.hpp"
#include "function.hpp"
#include "sequence.hpp"
#include "optimalization.hpp"
#include "mlp.hpp"
#include "datavisu.hpp"

using namespace std;

enum TestT
{
    eClassification,
    eRegression
};

int main(int argc, char *argv[])
{
    TestT testCase = eRegression;
//     TestT testCase = eClassification;

    if (testCase == eClassification)
    {
        Data data("data.tsv");
        vector<vector<double> > inputs;
        vector<int> targets;
        vector<int> inputIXS;
        int targetIX=1;
        inputIXS.push_back(0);
        data.extract(inputs,inputIXS);
        cout << "inputs:" << endl << Vector::toString(inputs) << endl;
        data.extractField(targets,targetIX);
        cout << "targets:" << endl << Vector::toString(targets) << endl;

        typedef FunctionFromClass<Neuron<eNeuronSigmoid>,double,0,1> NeuronF;
        Neuron<eNeuronSigmoid> n;
        NeuronF neuron(n);
        neuron.setInput(inputs[0]);
        vector<double> parameters;
        double v;
        parameters.push_back(1);
        parameters.push_back(1);
        neuron.setParameter(parameters);
        cout << neuron.output() <<  endl;

        PosteriorLogProb<NeuronF, vector<double>, int> plp(neuron,inputs,targets);
        plp.setInput(parameters);
        cout << plp.output() << endl;

        Optimizer<PosteriorLogProb<NeuronF, vector<double>, int> > optimizer;
        optimizer.optimize(parameters,plp,&v);
        cout << toString(parameters) << endl;
        cout << v << endl;

        // Learn an MLP on this data set
        vector<int> nrPerLayer;
        Vector::set(nrPerLayer,4,5,1);
        vector<NeuronE> typePerLayer;
        Vector::set(typePerLayer,eNeuronTanh,eNeuronTanh,eNeuronSigmoid);
        typedef MLP<double> MLPT;
        MLPT mlp(1,nrPerLayer,typePerLayer);
        vector<double> input;
        Vector::set(input,-2.0);
        MLPT::ParameterType *par;
        par=mlp.generateParameter();
        cout << Vector::toString(*par) << endl;
        mlp.setInput(input);
        mlp.setParameter(*par);

        PosteriorLogProb<MLPT, vector<double>, int> plpMLP(mlp,inputs,targets);
        plpMLP.setInput(*par);
        cout << plpMLP.output() << endl;

        Optimizer<PosteriorLogProb<MLPT, vector<double>, int> > optimizerMLP;
        optimizerMLP.optimize(*par,plpMLP,&v);
        cout << "optimize MLP parameters" << endl << toString(*par) << endl;
        cout << v << endl;

        DataVisu datavisu(1200,600);
        vector<double> xs,ys;
        int i;
    
        setEqual(xs,300,-6,6);
        ys.resize(300);
        mlp.setParameter(*par);
        for (i=0;i<xs.size();i++)
        {
            Vector::set(input,xs[i]);
            ys[i]=mlp.output(input);
        }
        datavisu.addLines(xs,ys);
        datavisu.show();
    }

    if (testCase == eRegression)
    {
        vector<vector<double> > inputs;
        vector<double> targets;

        // Create a sine pattern to learn
        int i,nrRecords=50;
        inputs.resize(nrRecords);
        Vector::setEqual(targets,nrRecords,-5,5);
        for (i=0;i<nrRecords;i++)
        {
            Vector::set(inputs[i],targets[i]);
            targets[i] = 0.85*sin(targets[i]);
        }

        // Learn an MLP on this data set
        double v;
        vector<int> nrPerLayer;
        Vector::set(nrPerLayer,4,5,1);
        vector<NeuronE> typePerLayer;
        Vector::set(typePerLayer,eNeuronTanh,eNeuronTanh,eNeuronLinear);
        typedef MLP<double> MLPT;
        MLPT mlp(1,nrPerLayer,typePerLayer);
        vector<double> input;
        Vector::set(input,-2.0);
        MLPT::ParameterType *par;
        par=mlp.generateParameter();
        cout << Vector::toString(*par) << endl;
        mlp.setInput(input);
        mlp.setParameter(*par);

        typedef PosteriorLogProb<MLPT, vector<double>, double> PLPT;
        PLPT plpMLP(mlp,inputs,targets);
        plpMLP.setInput(*par);
        cout << plpMLP.output() << endl;

        Optimizer<PLPT> optimizerMLP(10);
        optimizerMLP.optimize(*par,plpMLP,&v);
        cout << "optimize MLP parameters" << endl << toString(*par) << endl;
        cout << v << endl;

        DataVisu datavisu(1200,600);
        vector<double> xs,ys;

        // Draw the original curve
        xs.resize(inputs.size());
        ys = targets;        
        for (i=0;i<xs.size();i++)
            xs[i]=inputs[i][0];
        datavisu.addLines(xs,ys);

        // Draw the learned curve
        setEqual(xs,300,-6,6);
        ys.resize(300);
        mlp.setParameter(*par);
        for (i=0;i<xs.size();i++)
        {
            Vector::set(input,xs[i]);
            ys[i]=mlp.output(input);
        }
        datavisu.addLines(xs,ys);
        datavisu.show();
    }
    return 0;
}
