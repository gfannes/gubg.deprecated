require("gubg/utils")

class Root
    #These members can be filled-in in a root.tree file
    attr(:name, true)
    attr(:language, true)
    attr(:subtrees, true)
    attr(:settings, true)

    def initialize(string)
        #A hash of hashes of hashes: language, settings type, settings
        @language = Hash.new{|h, language|h[language] = Hash.new{|hh, kk|hh[kk] = {}}}
        eval(string, binding_)
    end
    def Root.createFromFile(fileName)
        Root.new(String.load(fileName))
    end

    def to_s
        res = ""
        res += "Name = #{@name}\n"
        @language.each do |language, settings|
            res += "Settings for #{language}:\n"
            settings.each do |k, v|
                res += "\t#{k} => #{v}\n"
            end
        end
        case @subtrees
        when String
            res += "Subtree: #{@subtrees}"
        when Array
            res += "Subtrees:\n"
            @subtrees.each{|subtree|res += "\t#{subtree}\n"}
        end
        res += "Settings = #{@settings}\n"
        res
    end

    private
    def binding_
        binding
    end
end

if __FILE__ == $0
   root = Root.new(DATA.read)
   puts("#{root}")
end

__END__
#Example root.tree file
@name = "test"
@language[:d][:compiler][:always] = "-I$HOME/d/src -I/home/befanneg/personal/d/src"
@language[:cpp] = {
    compiler: {
    always: "-I$GUBG/cpp/include",
    /ogre/ => "-I<path to Ogre3d>"
},
    linker: { always: "-L-lncurses -L-lcairo" }
}
@subtrees = ["../../root.tree", "Some other tree too"]
@settings = "../../root.tree"
