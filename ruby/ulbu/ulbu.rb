require("gubg")
require("parser")
require("walker")
require("root")

ulbuFile = "main.ulbu"
asmFile = ulbuFile + '.s'
objectFile = ulbuFile + '.o'
exeFile = ulbuFile + '.exec'

Dir.chdir("data") do
  root = Root.new

  subtree = time("\nParsing subtree #{ulbuFile}", true) do
    parser = Parser.new(UlbuParser)
    parser.parseFromFile(ulbuFile)
  end

  time("\nAdding subtree to root", true) do
    root.add(subtree, ["^"])
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
    walker.fileName = asmFile
    walker.walk(root)
  end

  time("\nAssembling and linking", true) do
    system("as #{asmFile} -o #{objectFile}")
    system("ld #{objectFile} -o #{exeFile}")
  end

  time("\nExecuting", true) do
    system("#{exeFile}")
  end
end
