class Token
	@@verbose = false
    def initialize(buffer, b = nil, e = nil)
        @buffer = buffer
        @range = [b || 0, e || @buffer.length]
    end
    def set(str)
        @buffer = str.dup
        @range = [0, @buffer.length]
    end
    def popNewline
        c = content
        if c[0, 2] == "\r\n"
            return split(2)
        elsif c[0, 1] == "\n"
            return split(1)
        else
            return nil
        end
    end
    def popFront(re = nil)
        if re
            case re
            when Regexp
                #Make sure we start matching from the start of the content buffer
                re = Regexp.new("\\A" + re.source, re.options) if re.source[0, 2] != "\\A"
                if md = re.match(content)
                    puts("Match for #{re}: (((#{md[0]})))") if @@verbose
                    return split(md[0].length)
                else
                    return nil
                end
            when String
                if re == content[0, re.length]
                    return split(re.length)
                else
                    return nil
                end
            else
                raise("No idea what to do with #{re}")
            end
        elsif block_given?
            res = nil
            transaction do
                res = yield(self)
                res.nil? ? :failure : :ok
            end
            res
        else
            raise("I expect either some matching string, or a block")
        end
    end
    def popUntil(str)
        ix = content.index(str)
        return nil if ix.nil?
        split(ix)
    end
    def length
        @range.last - @range.first
    end
    def split(nr)
        token = Token.new(@buffer, @range.first, @range.first+nr)
        forward(nr)
        token
    end
    def forward(nr)
        @range[0] += nr
    end
    def empty?()
        length == 0
    end
    def content
        @buffer[@range.first, length]
    end
    def to_s
        content
    end
    def hexdump(nr = nil)
        if nr
            content[0, nr]#.hexdump(false)
        else
            content#.hexdump(false)
        end
    end
    def transaction
        rangeSave = @range.dup
        case yield
        when :ok
        when :failure
            @range = rangeSave
        else
            raise("A transaction has to indicate if it failed or not")
        end
    end

    private
    def check_
        puts("@buffer.length: #{@buffer.length}, @range: #{@range}")
        if @range.any?{|r|r<0} or @range.first > @range.last
            raise("Range is not valid")
        end
        if @range.first >= @buffer.length or @range.last > @buffer.length
            raise("Range is out of range")
        end
        nil
    end
end
