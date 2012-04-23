require("targets.rb")
require("gubg/breakdown")
require("gubg/target")
require("gubg/options")
require("gubg/filestore")

options = parseOptions(name: "Generic build utility", author: "Geert Fannes", version: "0.1") do |parser, options|
    parser.on("-e", "--executable SOURCE", "Produce executable from SOURCE"){|filename|options[:executable] = filename}
    options[:run] = false
    parser.on("-r", "--run", "Run the produced executables"){options[:run] = true}
    parser.on("-c", "--clean", "Clean the filestore, forcing a complete rebuild"){options[:clean] = true}
    parser.on("-T", "--test-all", "Compile and run all tests"){options[:testAll] = true}
    parser.on("-p", "--program", "Program arduino"){options[:programArduino] = true}
end
$verbose = options[:verbose]

tmpDir = case operatingSystem
	 when /^Linux/ then "/home/gfannes/tmp"
	 when /^Windows/, /^Min/ then "c:/tmp"
	 end
puts("I will create the filestore in \"#{tmpDir}\"")
$filestore = FileStore.new(tmpDir, "gb-cache")
if options[:clean]
	puts("Cleaning the filestore \"#{$filestore.base}\"")
	$filestore.clean
end

global = Breakdown::Global.new do |global|
	if options[:executable]
		exe = global.breakdown(Executable.new(options[:executable]))
		if options[:run]
			global.breakdown(Run.new(exe.executable))
		end
        if options[:programArduino]
            global.breakdown(ProgramArduino.new(exe.executable))
        end
	end
    if options[:testAll]
        testAll = global.breakdown(TestAll.new(Dir.pwd))
    end
end

time("Processing GB") do
    global.process
end
