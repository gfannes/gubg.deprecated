require("targets.rb")
require("gubg/target")
require("gubg/options")

options = parseOptions(name: "Generic build utility", author: "Geert Fannes", version: "0.1") do |parser, options|
    parser.on("-e", "--executable SOURCE", "Produce executable from SOURCE"){|filename|options[:executable] = filename}
    options[:run] = false
    parser.on("-r", "--run", "Run the produced executables"){options[:run] = true}
end
$verbose = options[:verbose]

location = Location.new
configs = Configs.new
trees = Trees.new
sources = Sources.new
references = References.new
compileSettings = CompileSettings.new
linkSettings = LinkSettings.new
objectFiles = ObjectFiles.new
executables = Executables.new
run = Run.new
targets = [location, configs, trees, sources, references, compileSettings, linkSettings, objectFiles, executables]
targets << run if options[:run]
targetGraph = GUBG::TargetGraph.new(targets)

if options[:executable]
    filename = File.expand_path(options[:executable])
    location.set(File.dirname(filename))
    sources.add(filename)
end

def printWarnings(target)
    puts("Warnings for #{target.class}:")
    puts target.warnings
end

cycleCount = -1
loop do
    cycleCount += 1
    puts("\nRunning cycle #{cycleCount}")
    res = targetGraph.process
    puts("\tCycle #{cycleCount} resulted in \"#{res}\"")
    case res
    when :direct
    when :implicit
    when :error
        puts("Processing resulted in an error")
        targetGraph.getTargets(:error).each do |target|
            puts("Target #{target.class} resulted in error")
            printWarnings(target)
        end
        break
    else
	    puts("Finished: #{res}")
        break
    end
end
