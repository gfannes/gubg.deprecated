#Provides data aggregation over different dimensions
# * A dimension has a name => a symbol
# * The dimensions are fixed
# * A new dimension can be computed via computeNewDimension
# * A default aggregator can be specified
# * A selection of the cube can be asked/iterated
# * An aggregator can be used over a selection

require("stringio")

class Cube
    attr_reader(:dimensions, :valuesPerLoc)
    def initialize(*dimensions, &defaultAggregator)
        @dimensions = dimensions.flatten.sort
        setDefaultAggregator(&defaultAggregator)
        @valuesPerLoc = Hash.new do |h, loc|
            raise("Dimensions are not the same (#{loc.keys.sort} != #{@dimensions})") unless loc.keys.sort == @dimensions
            h[loc] = []
        end
    end
    def setDefaultAggregator(&block)
        @defaultAggregator = block
    end

    def add(value, loc = {})
        @valuesPerLoc[loc] << value
    end

    #Returns a new cube with an extra dimension
    def addDimensions(*dims, &block)
        res = Cube.new(@dimensions + dims, &@defaultAggregator)
        @valuesPerLoc.each do |loc, values|
            values.each do |value|
                loc = loc.dup
                tmp = [block.call(loc, value)].flatten
                dims.zip(tmp){|d, t|loc[d] = t}
                res.add(value, loc)
            end
        end
        res
    end
    def addDimensions!(*dims, &block)
        cube = addDimensions(*dims, &block)
        @dimensions = cube.dimensions
        @valuesPerLoc = cube.valuesPerLoc
    end

    #Splits the given cube into a cube-of-cube with outer dimensions as specified, and the inner dimensions as the rest
    def split(*outerDims)
        outerDims = outerDims.flatten
        raise("Unknown dimensions #{outerDims - @dimensions}") unless (outerDims - @dimensions).empty?
        raise("No dimensions given #{outerDims}") if outerDims.empty?
        innerDims = @dimensions - outerDims
        #By default, we will aggregate recursively over all subcubes
        res = Cube.new(*outerDims){|ary|@defaultAggregator.call(ary.map{|sc|sc.getAllValues}.flatten)}
        innerCubePerOuterLoc = Hash.new{|h, oloc|h[oloc] = Cube.new(innerDims, &@defaultAggregator)}
        @valuesPerLoc.each do |loc, values|
            outerLoc = loc.select{|dim, dv|outerDims.include?(dim)}
            innerLoc = loc.select{|dim, dv|innerDims.include?(dim)}
            innerCube = innerCubePerOuterLoc[outerLoc]
            values.each{|value|innerCube.add(value, innerLoc)}
        end
        innerCubePerOuterLoc.each do |oloc, icube|
            res.add(icube, oloc)
        end
        res
    end

    #Filter a cube into a new cube
    def select(&block)
        res = Cube.new(@dimensions, &@defaultAggregator)
        @valuesPerLoc.each do |loc, values|
            values.each do |value|
                res.add(value, loc) if block.call(loc, value)
            end
        end
        res
    end
    def reject(&block)
        res = Cube.new(@dimensions, &@defaultAggregator)
        @valuesPerLoc.each do |loc, values|
            values.each do |value|
                res.add(value, loc) unless block.call(loc, value)
            end
        end
        res
    end
    def reject!(&block)
        @valuesPerLoc.each do |loc, values|
            values.reject!{|value|block.call(loc, value)}
        end
	@valuesPerLoc.reject!{|loc, values|values.empty?}
	self
    end

    #Location values along a dimension
    def dimension(dim)
        raise("This cube has no such dimension (#{dim})") unless hasDimension?(dim)
        @valuesPerLoc.keys.map{|k|k[dim]}.uniq
    end

    def getAllValues()
        @valuesPerLoc.values.flatten
    end

    #Iteration
    def eachValue(&block)
        @valuesPerLoc.each do |loc, values|
            values.each{|value|block.call(loc, value)}
        end
    end
    def eachValues(&block)
        @valuesPerLoc.each do |loc, values|
            block.call(loc, values)
        end
    end
    def eachAggregate(&block)
        @valuesPerLoc.each do |loc, values|
            block.call(loc, @defaultAggregator.call(values))
        end
    end

    def aggregate(selection = {})
        ary = []
        if selection.empty?
            ary = getAllValues
        else
            split(selection.keys).eachValue do |loc, subCube|
                ary += subCube.getAllValues if loc == selection
            end
        end
        if block_given?
            yield(ary)
        else
            @defaultAggregator.call(ary)
        end
    end
    def collect(selection = {})
        ary = []
        if selection.empty?
            ary = getAllValues
        else
            split(selection.keys).eachValue do |loc, subCube|
                if loc == selection
                    ary += subCube.getAllValues
                end
            end
        end
        ary
    end

    def hasDimension?(dim)
        @dimensions.include?(dim)
    end
    def empty?
        @valuesPerLoc.empty?
    end

    def to_s
        "<<" + @valuesPerLoc.keys.map{|loc|"#{loc} -> #{@valuesPerLoc[loc]}"}*", " + ">>"
    end
end

if __FILE__ == $0
    cube = Cube.new(:file, :type){|ary|ary.inject{|s, v|s+v}}
    cube.add(1, type: :cpp, file: "main.cpp")
    cube.add(2, type: :cpp, file: "main.cpp")
    cube.add(1, type: :cpp, file: "lib.cpp")
    cube.add(1, type: :hpp, file: "main.hpp")
    puts("#{cube}")
    puts("#{cube.split(:type)}")
    #cube.add(1, typo: :hpp, file: "typo.hpp")
    puts cube.aggregate()
    puts cube.aggregate(type: :cpp)
    nc = cube.addDimensions(:size){|loc, value|(value > 1 ? :large : :small)}
    puts("nc: #{nc}")
    puts nc.aggregate()
    puts(nc.aggregate{|ary|ary.inject{|f, v|f*v}})
    puts nc.aggregate(type: :cpp)
    puts nc.aggregate(size: :small)
    nc2 = nc.select{|v, loc|v == 1}
    puts("#{nc2}")
end
