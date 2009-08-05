require("tools/utils")

class Dependency
  @@cache = Hash.new{|h, k|h[k] = {}}
  def Dependency.includedFiles(type, fileName)
    res = @@cache[type][fileName]
    if res.nil?
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
        re = /^(private|public|static)? *import +(.+);.*$/
        String.loadLines(fileName).each do |line|
          if md = re.match(line)
            res << md[2]
          end
        end
      else
        raise "Unknown type \"#{type}\""
      end
      @@cache[type][fileName] = res
    end
    res
  end
end
