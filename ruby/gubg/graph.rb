require("set")
class Graph
    Edge = Struct.new(:from, :to)
    Peer = Struct.new(:peer, :data)
    def initialize()
        @vertices = Set.new
        @edges = Hash.new
        @inPeers = Hash.new{|h, k|h[k] = []}
        @outPeers = Hash.new{|h, k|h[k] = []}
    end
    def addEdge(from, to, edgeData = nil)
        @vertices << from << to
        @edges[Edge.new(from, to)] = edgeData
        @outPeers[from] << Peer.new(to, edgeData)
        @inPeers[to] << Peer.new(from, edgeData)
    end
    def addVertex(v)
        @vertices << v
    end
    def vertices
        @vertices.to_a
    end
    def outVertices(v)
        @outPeers[v].map{|p|p.peer}
    end

    def to_s
        res = []
        @vertices.each do |v|
            res << "Vertex #{v}"
            [@outPeers, @inPeers].zip(%w[Out In]).each do |peers, tag|
                res << "\t#{tag}: " + peers[v].map{|p|p.data ? "#{p.peer} (#{p.data})" : "#{p.peer}"}.join(", ")
            end
        end
        res.join("\n")
    end
end

if $0 == __FILE__
    graph = Graph.new
    graph.addEdge(1, 2)
    graph.addEdge(1, 3)
    graph.addEdge(1, 4)
    graph.addEdge(3, 4)
    puts("#{graph}")
end
