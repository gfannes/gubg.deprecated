#Output to both file and stdout
class Output
    def initialize(fo)
        @fo = fo
    end
    def puts(str)
	    if @fo
		    @fo.puts(str)
		    @fo.flush
	    end
        Kernel.puts(str)
    end
    def Output.create(fileName)
        File.open(fileName + ".output", "wb") do |fo|
            yield(Output.new(fo))
        end
        nil
    end
end
