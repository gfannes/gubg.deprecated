require("tools/utils")

class LineBuffer
    def initialize(lines)
        @lines = (String === lines ? String.loadLines(lines) : lines)
        @ix = 0
    end
    def empty?
        @ix >= @lines.length
    end
    def peep
        @lines[@ix]
    end
    def pop
        @ix += 1
        @lines[@ix-1]
    end
end

