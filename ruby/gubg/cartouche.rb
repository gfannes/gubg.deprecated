require("stringio")

class Cartouche
    @@front = "@\#"
    @@back = "\#@"
    @@reCartouche = /@\#(.+)([:\+\?]=)([^\|]+)(\|.+)?\#@/

    def initialize()
        @data = Hash.new{|h, path|h[path] = []}
    end
    @@rePart = /(.+):(.+)/
    def Cartouche.create(line)
        res = nil
        if md = @@reCartouche.match(line)
            path, oper, value, filter = md[1], md[2], md[3], md[4]
            oper = case oper
                   when ":=" then :set
                   when "+=" then :append
                   when "?=" then :setIfUnset
                   else raise("Unexpected operation #{oper}")
                   end
            value = Cartouche.convert_(value)
            if filter
                tmp = filter[1, filter.length].split(/\s*,\s*/)
                filter = {}
                tmp.each do |part|
                    if mdd = @@rePart.match(part)
                        filter[mdd[1]] = Cartouche.convert_(mdd[2])
                    end
                end
            end
            res = Cartouche.new
            res.add_(path, value, oper, filter)
        end
        res
    end

    def <<(src)
        src.each do |path, info|
            @data[path] << info
        end
    end

    def each(&block)
        @data.each do |path, infos|
            infos.each{|info|block.call(path, info)}
        end
    end

    def to_s
        sio = StringIO.new
        @data.each do |path, infos|
            sio.puts(path)
            infos.each do |info|
                if info[:filter]
                    sio.puts("\t#{info[:oper]}\t#{info[:value]}\tif #{info[:filter]}")
                else
                    sio.puts("\t#{info[:oper]}\t#{info[:value]}")
                end
            end
        end
        sio.string
    end

    #	private
    def add_(path, value, oper, filter)
        @data[path] << {value: value, oper: oper, filter: filter}
    end
    @@reNumber = /^\d+$/
    def Cartouche.convert_(str)
        case str
        when @@reNumber then str.to_i
        else str
        end
    end
end

if __FILE__ == $0
    str = %q!
@#compiler.includePaths+=path#@
@#compiler.includePaths+=path_linux|platform:linux#@
@#compiler.includePaths+=path_linux_64|platform:linux,bits:64#@
@#p:=v#@
@#p?=123#@
@#p:=124#@
    !
    all = Cartouche.new
    str.split("\n").each do |line|
        if c = Cartouche.create(line)
            puts("#{c}")
            all << c
        else
            puts("ERROR::Could not create cartouche from (#{line})")
        end
    end
    puts("all:\n#{all}")
end
