require("stringio")

class Cartouche
    @@front = "@\#"
    @@back = "\#@"
    @@reCartouche = /@\#(.+)([:\+\?]=)([^\|]+)(\|.+)?\#@/

    def initialize()
        @infosPerPath = Hash.new{|h, path|h[path] = []}
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
                        filter[Cartouche.convert_(mdd[1])] = Cartouche.convert_(mdd[2])
                    end
                end
            end
            res = Cartouche.new
            res.add_(path, value, oper, filter)
        end
        res
    end
    def Cartouche.createFrom(na = {file: nil, line: nil})
        res = Cartouche.new
        if na.has_key?(:file)
            String.loadLines(na[:file]).each do |line|
                if c = Cartouche.create(line)
                    res << c
                end
            end
        elsif na.has_key?(:line)
            res = Cartouche.create(na[:line])
        else
            raise("Could not create a cartouche from #{na}")
        end
        res
    end

    def <<(src)
        src.each do |path, info|
            @infosPerPath[path] << info
        end
    end

    def each(&block)
        @infosPerPath.each do |path, infos|
            infos.each{|info|block.call(path, info)}
        end
    end

    #env is a hash of key-value pairs that define the environment to be used for evaluation
    def getInfo(env)
        env = Cartouche.convertEnv_(env)
        res = {}
        @infosPerPath.each do |path, infos|
            infos.each do |info|
                #Check if this info falls in the current environment
                next unless environmentAllows_?(env, info)
                #Process the info
                case info[:oper]
                when :set
                    res[path] = info[:value]
                when :append
                    case res[path]
                    when NilClass then res[path] = [info[:value]]
                    when Array then res[path] << info[:value]
                    else res[path] = [res[path], info[:value]] end
                when :setIfUnset
                    case res[path]
                    when NilClass then res[path] = info[:value]
                    else end
                else raise("Unknown operator #{info[:oper]}") end
            end
        end
        res
    end

    def to_s
        sio = StringIO.new
        @infosPerPath.each do |path, infos|
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
        @infosPerPath[path] << {value: value, oper: oper, filter: filter}
    end
    #For now, we convert everything either to a number or a string
    @@reNumber = /^\d+$/
    def Cartouche.convert_(str)
        str = str.to_s
        case str
        when @@reNumber then str.to_i
        else str end
    end
    def Cartouche.convertEnv_(env)
        res = {}
        env.each{|k, v|res[convert_(k)] = convert_(v)}
        res
    end
    def environmentAllows_?(env, info)
        #We allow blind if no filter is set
        return true unless info[:filter]

        info[:filter].each do |k, v|
            return false if env.has_key?(k) and env[k] != v
        end
        true
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
