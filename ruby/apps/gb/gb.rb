require("targets.rb")
require("gubg/breakdown")
require("gubg/target")
require("gubg/options")

options = parseOptions(name: "Generic build utility", author: "Geert Fannes", version: "0.1") do |parser, options|
    parser.on("-e", "--executable SOURCE", "Produce executable from SOURCE"){|filename|options[:executable] = filename}
    options[:run] = false
    parser.on("-r", "--run", "Run the produced executables"){options[:run] = true}
end
$verbose = options[:verbose]

global = Breakdown::Global.new do |global|
    if options[:executable]
        exe = global.generate(Executable.new(options[:executable]))
        if options[:run]
            global.generate(Run.new(exe.executable))
        end
    end
end
global.process
