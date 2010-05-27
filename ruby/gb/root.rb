require("gubg/utils")

class Root
    #These members can be filled-in in a root.tree file
    attr(:name, true)
    attr(:compiler, true)
    attr(:linker, true)
    attr(:subtrees, true)
    attr(:settings, true)

    def initialize(string)
        @compiler = Hash.new{|h, language|h[language] = {}}
        @linker = Hash.new{|h, language|h[language] = {}}
        eval(string, binding_)
    end
    def Root.createFromFile(fileName)
        Root.new(String.load(fileName))
    end

    def to_s
        res = ""
        res += "Name = #{@name}\n"
        @compiler.each do |language, settings|
            res += "Compiler settings for #{language}:\n"
            settings.each do |k, v|
                res += "\t#{k} => #{v}\n"
            end
        end
        @linker.each do |language, settings|
            res += "Linker settings for #{language}:\n"
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
@compiler[:d][:always] = "-I$HOME/d/src -I/home/befanneg/personal/d/src"
@compiler[:cpp][:always] = "-I$GUBG/cpp/include"
@compiler[:cpp][/ogre/] = "-I<path to Ogre3d>"
@linker[:d][:always] = "-L-lncurses -L-lcairo"
@subtrees = ["../../root.tree", "Some other tree too"]
@settings = "../../root.tree"
