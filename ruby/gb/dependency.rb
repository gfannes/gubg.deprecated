require("gubg/utils")

class Dependency
    protected
    def parsePerLine_(filename, &block)
        (@cache ||= {})[filename] ||= [].tap do |res|
            String.loadLines(filename).each{|line|block.call(line, res)}
        end.uniq
    end
end

class DDependency < Dependency
    def importedModules(filename)
        parsePerLine_(filename) do |line, modulesAry|
            if md = (@@reImport ||= /^(private|public|static)?\s*import +(.+);.*$/).match(line)
                modName = md[2]
                # Check for renamed imports
                (@@reRenamedImport ||= /(.+)=(.+)/).match(modName).tap{|md2|modName = md2[2] if md2}
                modulesAry << modName.strip
            end
        end
    end
end

class CPPDependency < Dependency
    def includedFiles(filename)
        parsePerLine_(filename) do |line, modulesAry|
            (@@reInclude ||= /^\#include +[<\"](.+)[>\"]/).match(line).tap{|md|modulesAry << md[1] if md}
        end
    end
end

if __FILE__ == $0
    puts DDependency.new.importedModules("/home/gfannes/gubg/d/gubg/graphics/Canvas.d")
end
