require("tools")
require("parser")
require("walker")

fileNames = [
  "data/main.ulbu",
  "data/test.ulbu",
]

fileNames.each do |fileName|
  puts("\nfileName = #{fileName}")
  tree = time("Parsing #{fileName}", true) do
    parser = Parser.new(UlbuParser)
    parser.parse(fileName)
  end
  
  puts("")
  
  time("Printing", true) do
    walker = Walker.new(UlbuWalker)
    walker.walk(tree)
  end
end
