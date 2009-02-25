require("tools/utils")

class Dependency
  def Dependency.includedFiles(fileName)
    res = []
    re = /^\#include +[<\"](.+)[>\"]/
    String.loadLines(fileName).each do |line|
      case line
      when re
          res << line[re, 1]
      end
    end
    res
  end
end
