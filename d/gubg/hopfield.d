module gubg.hopfield;

import gubg.neuron;
import gubg.random;
import gubg.puts;

class Hopfield
{
    const real biasInit = 0.5;

    this(int nrNodes)
        {
            mNeurons.length = nrNodes;
            // Create the neurons
            foreach(int ix, inout node; mNeurons)
                node = new Neuron(ix, uniform(-biasInit, biasInit));
	}

    void createFullStructure()
	{
            // Create the fully connected structure
            foreach(destination; mNeurons)
            {
                foreach(source; mNeurons)
                {
                    if (source != destination)
                        destination.addSynaps(source, 1.23);
                }
            }
        }

    // Simulate the hopfield network
    void updateValues(real dt)
	{
	    foreach (n; mNeurons)
		n.updateValue(dt);
	}

    // Update the weights to make the neurons achieve their targets better
    void updateWeights(real dt)
	{
	    foreach (n; mNeurons)
		n.updateWeights(dt);
	}

    // Update target values for each neuron to help their neighbours achieve their targets
    void updateTargets(real dt)
	{
	    // Clear the partial targets
	    foreach (n; mNeurons)
		n.clearPartialTargets;

	    // Provide the partial targets to the neurons
	    foreach (n; mNeurons)
		n.providePartialTargets;

	    // Compute the new targets from the partial targets
	    foreach (n; mNeurons)
		n.updateTarget(dt);
	}
    
    // Iteration over the neurons
    void eachNeuron(void delegate(int ix, Neuron neuron) callback)
	{
	    foreach (ix, neuron; mNeurons)
		callback(ix, neuron);
	}

    ulong nrSynapses()
	{
	    ulong nr = 0;
	    foreach (n; mNeurons)
		nr += n.nrSynapses();
	    return nr;
	}
    
    // []-method
    Neuron opIndex(int ix)
	{
	    return mNeurons[ix];
	}

    void print()
        {
            foreach(node; mNeurons)
                node.print();
        }
private:
    Neuron[] mNeurons;
}

version (UnitTest)
{
    void main()
    {
        Hopfield h = new Hopfield(10);
        h.print();
    }
}