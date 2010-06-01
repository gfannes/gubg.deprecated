require("optparse")
require("tree")

$version = [0, 0, 1]

options = {}
OptionParser.new do |parser|
    parser.banner = "Usage: gb <options> [location*]"
    
    parser.separator("")
    parser.separator("Build options:")
    parser.separator("    location*                        The location(s) of the affected trees. Working directory is default.")
    options[:clean] = false
    parser.on("-c", "--clean", "Clean all object files"){options[:clean] = true}

    parser.separator("")
    parser.separator("Source code options:")
    options[:git] = nil
    gitCommands = %w[push pull status commit]
    parser.on("-g", "--git COMMAND", "Git COMMAND can be: #{gitCommands.join(', ')}") do |command|
        case command
        when *gitCommands
            options[:git] = command.to_sym
        else
            puts("Unknown git command \"#{command}\". Possible commands are: #{gitCommands.join(', ')}.")
            exit(0)
        end
    end
    options[:format] = false
    parser.on("-f", "--format", "Format the source code"){options[:format] = true}
    options[:create] = false
    parser.on("-C", "--create", "Create a default \"#{Tree.rootFileName}\" file"){options[:create] = true}

    parser.separator("")
    parser.separator("Common options:")
    options[:verbose] = false
    parser.on("-v", "--verbose", "Be verbose"){options[:verbose] = true}
    parser.on("-V", "--version", "Print version") do
        puts("Version #{$version.join('.')}")
        exit(0)
    end
    parser.on("-h", "--help", "Print help") do
        puts(parser)
        exit(0)
    end

    parser.separator("")
    parser.separator("Created by Geert Fannes under GPL.")
end.parse!

options[:locations] = ARGV.empty? ? [Dir.pwd] : ARGV.collect{|v|File.expand_path(v)}

$verbose = options[:verbose]
if $verbose
    puts("Options: #{options}")
end
