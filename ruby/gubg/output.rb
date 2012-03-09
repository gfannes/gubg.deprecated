#Output to both file and stdout
class Output
    def initialize(fo)
        @fo = fo
    end
    def puts(str)
        @fo.puts(str) if @fo
        Kernel.puts(str)
    end
    def Output.create(fileName)
        File.open(fileName + ".output", "wb") do |fo|
            yield(Output.new(fo))
        end
        nil
    end
end
