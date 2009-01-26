require("tools")
require("parser")
require("walker")
require("root")

fileName = "main.ulbu"

Dir.chdir("data") do
  root = Root.new

  subtree = time("\nParsing subtree #{fileName}", true) do
    parser = Parser.new(UlbuParser)
    parser.parse(fileName)
  end

  time("\nAdding subtree to root", true) do
    root.add(subtree, ["/"])
  end

  time("\nResolving", true) do
    walker = Walker.new(ResolveWalker)
    walker.walk(root)
  end

  time("\nPrinting", true) do
    walker = Walker.new(PrintWalker)
    walker.walk(root)
  end


  time("\nCompiling", true) do
    walker = Walker.new(CompileWalker)
    walker.fileName = fileName + '.s'
    walker.walk(root)
  end
end
