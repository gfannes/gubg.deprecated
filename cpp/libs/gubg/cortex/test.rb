class Neuron
    attr_accessor(:bias, :weights)
    def compute(inputs)
        res = bias
        @weights.zip(inputs){|w, i|bias + w*i}
        res = 1.0-2.0/(1.0+Math::exp(res+res));
        res
    end
end

nr = 1000000
neurons = (0..nr).map do
    n = Neuron.new
    n.bias = 0.5
    n.weights = [0.1, 0.2, 0.3, 0.4]
    n
end
inputs = [1, 2, 3, 4]
10.times do
    puts("iteration")
    neurons.each do |neuron|
        neuron.compute(inputs)
    end
end
