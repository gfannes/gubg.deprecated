class Parser
  def initialize(mod)
    extend(mod)
  end

  def parse(fileName)
    @globalBuffer = String.load(fileName)
    @baseName = File.basename(fileName, ".ulbu")
    @dirName = File.dirname(fileName)
    @ixs = [0]
    prepareParsing
    res = complete
    finishedParsing
    res
  end

  def buffer
    @globalBuffer[@ixs.last, @globalBuffer.length]
  end

  def matches(s, e = nil)
    case s
    when String
      if buffer[0, s.length] == s
        consume(s.length)
        yield(s) if block_given?
        matches(e) if e
        return true
      end
    when Regexp
      m = s.match(buffer)
      if m
        consume(m[0].length)
        yield(m[m.length-1]) if block_given?
        matches(e) if e
        return true
      end
    end
    return false
  end

  def consume(nr)
    throw "ERROR::I cannot consume when I'm not parsing" if @ixs.length <= 1
    @ixs[-1] = @ixs[-1] + nr
    while (buffer[0, 1] == ' ' or buffer[0, 1] == "\n" or buffer[0, 1] == "\t")
      @ixs[-1] += 1
    end
  end
  
  def complete(obj = nil)
    @ixs << @ixs.last
    res = complete4Parser(obj)
    @ixs[-2] = @ixs[-1] if res
    @ixs.pop
    res
  end
end
