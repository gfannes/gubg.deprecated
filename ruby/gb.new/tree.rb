class Tree
  attr :rootTree, true
  attr :base, true
  attr :file, true
  def initialize(pwd)    
    @base, @file = Tree.findBaseFile(pwd, interestedFiles)
    case self
    when RootTree
      @rootTree = self
      else
      @rootTree = RootTree.new(pwd)
    end
  end

  def Tree.findBaseFile(pwd, files)
    base, file = nil, nil
    # Recursively descend until one of these files has been found
    here = pwd
    while base.nil?
      # Try to find a match
      files.each do |lfile|
        if File.exist?(File.expand_path(lfile, here))
          base = here
          file = lfile
          puts("I found basefile \"#{file}\" at \"#{base}\"")
          break
        end
      end
      break if !base.nil?
      # No match was found, decrease "here"
      raise "ERROR::Could not find any basefile from \"#{pwd}\" (#{files})" if here == "/"
      here = File.dirname(here)
    end
    return base, file
  end

  def interestedFiles
    %w[root.tree main.cpp main.d main.rb test.cpp test.d test.rb]
  end
end

class RootTree < Tree
  def initialize(pwd)
    super(pwd)
  end
  def pushURL
    raise "Not implemented"
  end
  def pullURL
    raise "Not implemented"
  end

  def interestedFiles
    %w[root.tree]
  end
end

if __FILE__ == $0
  tree = Tree.new(Dir.pwd)
end
