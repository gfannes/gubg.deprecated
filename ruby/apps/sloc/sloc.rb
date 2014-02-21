require("gubg/cube")
require("gubg/options")
require("gubg/utils")

DEFAULT_EXTS = ["hpp", "cpp", "hxx", "cxx", "h", "rb", "d", "c"]
options = parseOptions(name: "Lines of source counter", author: "Geert Fannes") do |parser, options|
    options[:extensions] = []
    parser.on("-e", "--extension EXT", "Add extension (without dot)"){|ext|options[:extensions] << ext}
    options[:keywords] = []
    parser.on("-k", "--keywords WORD", "Search for keywords"){|keyword|options[:keywords] << keyword}
end

if options[:extensions].empty?
    DEFAULT_EXTS.each{|ext|options[:extensions] << ext}
end

sloc = Cube.new(:file, :dir, :ext){|ary|ary.sum}
keywords = Cube.new(:file, :dir, :ext, :keyword){|ary|ary.sum}
reExtension = Regexp.new("\.(#{options[:extensions]*"|"})$")
rePerKeyword = {}
options[:keywords].each{|keyword|rePerKeyword[keyword] = /\b#{keyword}\b/}
puts("#{rePerKeyword}")
Dir.each do |dir, fn|
	case dir
	when /CVS/, /win32/
	else
		if md = reExtension.match(fn)
			ext = md[1]
			filename = File.expand_path(fn, dir)
			lines = String.loadLines(filename)
			sloc.add(lines.length, file: filename, dir: dir, ext: ext)
			options[:keywords].each do |keyword|
				nr = lines.map{|line|line.scan(rePerKeyword[keyword]).length}.sum || 0
				keywords.add(nr, file: filename, dir: dir, ext: ext, keyword: keyword)
			end
		end
	end
    :ok
end

[sloc, keywords].zip(%w[sloc keywords]).each do |cube, str|
	unless cube.empty?
		puts("Total #{str}: #{cube.aggregate}")
		[:ext, :dir, :keyword].each do |sym|
			cube.dimension(sym).each do |v|
				puts("\t#{v}\t#{cube.aggregate(sym => v)}")
			end if cube.hasDimension?(sym)
		end
	end
end
