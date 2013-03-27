import java.util.*;
import java.lang.Math;

class Neuron
{
    double bias;
    double[] weights;
    public void setBias(double b){bias = b;}
    double compute(double[] inputs)
    {
        double res = bias;
        for (int i = 0; i < weights.length; ++i)
            res += weights[i]*inputs[i];
        res = 1.0-2.0/(1.0+Math.exp(res+res));
        return res;
    }
}
class Neurons
{
    private ArrayList<Neuron> neurons;
    Neurons(int nr)
    {
        neurons = new ArrayList<Neuron>(nr);
        for (int i = 0; i < nr; ++i)
            neurons.add(new Neuron());
    }
    void setBiasAndWeights(double bias, double[] weights)
    {
        for (int i = 0; i < neurons.size(); ++i)
        {
            neurons.get(i).bias = bias;
            neurons.get(i).weights = (double[])weights.clone();
        }
    }
    double compute(double[] inputs)
    {
        double d = 0;
        for (int i = 0; i < neurons.size(); ++i)
        {
            d = neurons.get(i).compute(inputs);
        }
        return d;
    }
}
public class test
{
    public static void main(String[] args)
    {
        Neurons neurons = new Neurons(30000000);
        double[] weights = {0.1, 0.2, 0.3, 0.4};
        neurons.setBiasAndWeights(0.5, weights);
        double[] inputs = {1, 2, 3, 4};
        double sum = 0;
        for (int i = 0; i < 10; ++i)
        {
            System.out.println("Iteration " + i);
            sum += neurons.compute(inputs);
        }
        System.out.println("sum: " + sum);
        System.out.println("Finished");
    }
}
