require("gubg/utils")

class Root
    attr(:name, true)
    attr(:compilation, true)
    attr(:linking, true)
    attr(:subtrees, true)
    attr(:extraSettings, true)

    def initialize(string)
        @compilation = Hash.new{|h, language|h[language] = {}}
        @linking = Hash.new{|h, language|h[language] = {}}
        eval(string, binding_)
    end
    def Root.createFromFile(fileName)
        Root.new(String.load(fileName))
    end

    def to_s
        res = ""
        res += "name = #{@name}\n"
        @compilation.each do |language, settings|
            res += "Compilation settings for #{language}:\n"
            settings.each do |k, v|
                res += "\t#{k} => #{v}\n"
            end
        end
        @linking.each do |language, settings|
            res += "Linking settings for #{language}:\n"
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
        res += "extraSettings = #{@extraSettings}\n"
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
@name = "test"
@compilation[:d][:always] = "-I$HOME/d/src -I/home/befanneg/personal/d/src"
@compilation[:cpp][:always] = "-I$GUBG/cpp/include"
@compilation[:cpp][/ogre/] = "-I<path to Ogre3d>"
@subtrees = ["../../root.tree", "Some other tree too"]
