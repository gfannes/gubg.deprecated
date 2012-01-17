#To use this parser, do:
# * Create a module with methods
#    * complete4Parser(obj)
#       * Should create or complete obj, and return the finalized result
#       * Don't call this directly yourself recursively, call complete() instead
#    * whitespaces
#       * Should return an array of strings to be used as whitespace
#    * prepareParsing (optional)
#    * finishedParsing (optional)
# * Be careful with using "^" as an indication of "beginning of string". "^" just means "after newline", use "\A" to indicate beginning of a String
# * For the end of a string, use "\Z"

require("gubg/utils")

class Parser
	def initialize(mod)
		extend(mod)
	end

	def parseFromFile(fileName, obj = nil)
		parseFromString(String.load(fileName))
	end
	def parseFromString(string, obj = nil)
		@globalBuffer = string
		@ixs = [0]
		setWhitespace_(whitespaces)
		prepareParsing if self.methods.include?(:prepareParsing)
		res = complete(obj)
		finishedParsing if self.methods.include?(:finishedParsing)
		res
	end

	def buffer
		@globalBuffer[@ixs.last, @globalBuffer.length]
	end
	def finished?
		eatWhitespace_
		(@globalBuffer.length - @ixs.last) == 0
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
				consume(m.offset(0)[1])
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
		eatWhitespace_
	end

	def complete(obj = nil)
		eatWhitespace_
		@ixs << @ixs.last
		res = complete4Parser(obj)
		@ixs[-2] = @ixs[-1] if res
		@ixs.pop
		res
	end

	#Private methods
	def setWhitespace_(whitespaces)
		@whitespaces = whitespaces
	end
	def isWhitespace_
		buf = buffer
		@whitespaces.each do |ws|
			return ws if buf[0, ws.length] == ws
		end
		nil
	end
	def eatWhitespace_
		whitespace = nil
		while (whitespace = isWhitespace_)
			@ixs[-1] += whitespace.length
		end
	end
end
