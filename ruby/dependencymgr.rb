require("tools/utils")

class DependencyMgr
  def initialize(fileName)
    if !File.exist?(fileName)
      puts("Could not find build index \"#{fileName}\", creating empty one.")
      DependencyMgr.createEmpty(fileName)
    end
    loadFrom(fileName)
  end

  def computeInfo(fileName)
    case fileName
    when /\.cpp$/
      
    end
  end

  def outdated?(fileName)
    res = true
    # Get the stored information
    infoS = getInfo(fileName)
    if !info.nil?
      # Get the real information
      infoC = computeInfo(fileName)
      res = !(infoS === infoC)
    end
    res
  end

  def update(fileName)
    setInfo(fileName, computeInfo(fileName))
  end

  def getInfo(fileName)
    info = nil
    fullName = File.expand_path(fileName)
    dir, fn = File.dirname(fullName), File.basename(fullName)
    info = @dirfn2Info[dir][fn] if @dirfn2Info.has_key?(dir)
    info
  end
  def setInfo(fileName, info)
    fullName = File.expand_path(fileName)
    dir, fn = File.dirname(fullName), File.basename(fullName)
    @dirfn2Info[dir][fn] = info
  end

  def DependencyMgr.createEmpty(fileName)
    File.open(fileName, "w") do |fo|
    end
  end

  class FileInfo
    attr :name, true
    attr :checksum, true
    attr :size, true
    attr :date, true
  end

  def loadFrom(fileName)
    @dirfn2Info = Hash.new{|h, k|h[k] = {}}
    String.loadLines(fileName) do |line|
      puts("line = #{line}")
    end
  end
end

if __FILE__ == $0
  dmgr = DependencyMgr.new("test.tmp")
  puts dmgr.outdated?("../cpp/common/visualization/visualization.cpp")
end
