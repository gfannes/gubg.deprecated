require("gubg/cube")
require("gubg/options")
require("gubg/utils")

DEFAULT_EXTS = ["hpp", "cpp", "h", "rb", "d"]
options = parseOptions(name: "Lines of source counter", author: "Geert Fannes") do |parser, options|
    options[:extensions] = []
    parser.on("-e", "--extension EXT", "Add extension (without dot)"){|ext|options[:extensions] << ext}
end

if options[:extensions].empty?
    DEFAULT_EXTS.each{|ext|options[:extensions] << ext}
end

sloc = Cube.new(:file, :ext){|ary|ary.sum}
re = Regexp.new("\.(#{options[:extensions]*"|"})$")
Dir.each do |dir, fn|
    if md = re.match(fn)
        filename = File.expand_path(fn, dir)
        sloc.add(String.loadLines(filename).length, file: filename, ext: md[1])
    end
    :ok
end

puts("Total sloc: #{sloc.get}")
puts("Sloc per extension")
options[:extensions].each do |ext|
    puts("\t#{ext}\t#{sloc.get(ext: ext)}")
end
