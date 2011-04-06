require("gubg/utils")

def finalize(msg)
    puts("#{msg}")
    exit(-1)
end

options = {clean: true}

if options[:clean]
finalize("Could not clean gb") if !system("gb clean")
end
Dir.each do |dir, fn|
    case fn
    when /\.cpp$/
        Dir.chdir(dir) do
        finalize("Failed to build #{fn}") if !system("gb unit #{fn}")
        system("./#{File.basename(fn, '.cpp')}.unit")
        end
    end
    :ok
end
