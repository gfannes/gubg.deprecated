#Provides data aggregation over different dimensions

class Cube
    def initialize(*dimensions, &mergeBlock)
        @dimensions = dimensions.sort
        @mergeBlock = mergeBlock
        @data = {} 
    end
    def add(value, dims = {})
        raise("Dimensions are not the same (#{dims.keys.sort} != #{@dimensions})") unless dims.keys.sort == @dimensions
        @data[dims] = value
    end
    def get(dims = {})
        ary = []
        keys, vals = dims.keys, dims.values
        @data.each do |k, v|
            valsAt = k.values_at(*dims.keys)
            if k.values_at(*dims.keys) == vals
                ary << v
            end
        end
        @mergeBlock.call(ary)
    end
end

if __FILE__ == $0
    cube = Cube.new(:file, :type){|ary|ary.inject{|s, v|s+v}}
    cube.add(1, type: :cpp, file: "main.cpp")
    cube.add(1, type: :cpp, file: "lib.cpp")
    cube.add(1, type: :hpp, file: "main.hpp")
    #cube.add(1, typo: :hpp, file: "typo.hpp")
    puts cube.get()
    puts cube.get(type: :cpp)
end
