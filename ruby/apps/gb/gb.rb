require("./targets.rb")
require("gubg/graph")
require("gubg/options")

options = parseOptions(name: "gb", author: "Geert Fannes", version: "0.1") do |parser, options|
end
$verbose = options[:verbose]

locations = Locations.new
configs = Configs.new
trees = Trees.new
cppFiles = CppFiles.new
hppFiles = HppFiles.new
compileSettings = CompileSettings.new
linkSettings = LinkSettings.new
objectFiles = ObjectFiles.new
executables = Executables.new

graph = Graph.new
graph.addEdge(configs, locations)
graph.addEdge(trees, configs)
graph.addEdge(hppFiles, trees)
graph.addEdge(hppFiles, cppFiles)
graph.addEdge(cppFiles, trees)
graph.addEdge(cppFiles, hppFiles)
graph.addEdge(compileSettings, configs)
graph.addEdge(compileSettings, trees)
graph.addEdge(compileSettings, hppFiles)
graph.addEdge(compileSettings, cppFiles)
graph.addEdge(linkSettings, configs)
graph.addEdge(objectFiles, compileSettings)
graph.addEdge(objectFiles, cppFiles)
graph.addEdge(executables, linkSettings)
graph.addEdge(executables, objectFiles)

loop do
    #Collect all unfinished targets
    unfinishedTargets = graph.vertices.reject{|v|v.state == :generated}
    break if unfinishedTargets.empty?

    #Collect all targets that can be generated directly
    generatableTargets = unfinishedTargets.select{|t|graph.outVertices(t).all?{|v|v.state == :generated}}
    if !generatableTargets.empty?
        puts("The following targets are generatable #{generatableTargets}")
        generatableTargets.each{|t|t.generate(graph.outVertices(t))}
        next
    end

    #Collect all progressible targets
    progressibleTargets = unfinishedTargets.select{|t|t.progressible?(graph.outVertices(t))}
    if !progressibleTargets.empty?
        puts("The following targets are generatable #{progressibleTargets}")
        progressibleTargets.each{|t|t.generate(graph.outVertices(t))}
        next
    end

    finalize("Unable to find a target to work on", :error)
end

puts("#{graph}")
