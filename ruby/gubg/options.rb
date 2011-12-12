require("optparse")

def parseOptions(na = {name: nil, brief: nil, author: nil, version: nil}, &block)
	options = {}
	header = na[:name]
	header += ": #{na[:brief]}" if header and na[:brief]
	footer = na[:author]
	version = na[:version] || "0.0.1"
	showHelp = false
	showVersion = false
	parser = OptionParser.new do |parser|
		parser.banner = header if header
		parser.on("-h", "--help", "Show this help"){showHelp = true}
		parser.on("-V", "--version", "Show the version"){showVersion = true}
		parser.on("-v", "--verbose", "Switch on verbose mode"){$verbose = true}
		yield(parser, options)
		parser.separator(footer) if footer
	end
	parser.parse!
	if showHelp
		puts(parser)
		exit(0)
	end
	if showVersion
		puts("Version: #{version}")
		exit(0)
	end
	options
end

def finalize(msg, code)
    prefix = case code
             when :ok then ""
             else "#{code.to_s.upcase}::"
             end
    puts("#{prefix}#{msg}")
    exit(code == :ok ? 0 : -1)
end

if __FILE__ == $0
	options = parseOptions(author: "Geert Fannes") do |parser, options|
	end
	puts("options: #{options}")
end
