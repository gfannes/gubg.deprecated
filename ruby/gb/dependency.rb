require("tools/utils")

class Dependency
  def Dependency.includedFiles(type, fileName)
    res = []
    case type
    when :cpp
      re = /^\#include +[<\"](.+)[>\"]/
      String.loadLines(fileName).each do |line|
        case line
        when re
          res << line[re, 1]
        end
      end
    when :d
      re = /^(private|public)? *import +(.+);.*$/
      String.loadLines(fileName).each do |line|
        if md = re.match(line)
          res << md[2]
        end
      end
    else
      raise "Unknown type \"#{type}\""
    end
    res
  end
end
