require("gubg/options")
require("gubg/utils")

options = parseOptions(name: "Compiles a gollum asciidoc wiki into a static website", author: "Geert Fannes", version: "0.1") do |parser, options|
    options[:output] = "smeagol"
    parser.on("-o", "--output DIR", "Output directory (default #{options[:output]})"){|dir|options[:output] = dir}
end
$verbose = options[:verbose]

Dir.mkdir(options[:output]) unless File.exist?(options[:output])

infoPerPage = {}
Dir.each("./") do |dir, fn|
    case fn
    when /\.asciidoc$/
        fp = "#{dir}/#{fn}"
        puts("Processing #{fp}")
        page = Dir.relative(fp.gsub(/\.asciidoc$/, ""))
        html = File.expand_path(page.gsub("/", "_"), options[:output])
        html.setExtension!("html")
        #system("asciidoc -b html5 -o #{html} #{fp}")
        system("asciidoctor -b html5 -o #{html} #{fp}")
        infoPerPage[page] = {html: html}
    else
    end
    :ok
end

#Fix all internal links
reLink = /<a href=\"([a-zA-Z\d\-_]+)\">/
infoPerPage.each do |page, info|
    content = String.load(info[:html])
    content.gsub!(reLink) do |link|
        if md = reLink.match(link)
            if infoPerPage.has_key?(md[1])
                "<a href=\"#{md[1].gsub("/", "_")}.html\">"
            else
                link
            end
        else
            raise("Expected to find a link")
        end
    end
    content.export(info[:html])
    content.export(File.expand_path("index.html", options[:output])) if page == "home"
end if false
