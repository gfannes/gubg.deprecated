require("yaml")

require("patterns/chainOfResponsibility")
require("gubg/utils")
require("gubg/filestore")

$verbose = false

class Tree# < IChainOfResponsibility
  attr :rootTree, true
  attr :base, true
  attr :target, true
  attr :successor, true
  @@definingFiles = %w[root.tree disabled.tree main.cpp Main.d test.cpp Test.d test.rb]
  @@cppFile = /\.cpp$/
  @@hppFile = /\.hpp$/
  @@dFile = /\.d$/
  @@wantedFiles = [@@cppFile, @@hppFile, @@dFile]
  @@fileStore = FileStore.new("/tmp/gb")
  @@linkerPerType = {cpp: "g++", d: "dmd"}
  def initialize(base, file, target)
    @base, @settingsFile, @target = base, file, target
    @target = nil if target == false
    loadSettings
    @dirsPerFile = Hash.new{|h, k|h[k] = []}
    each do |dir, fn|
      @dirsPerFile[fn] << dir
    end
  end

  def buildCommands(command = nil)
    commands = []
    case command
    when NilClass
      case @target

      when NilClass
        # Compile all object files
        type = nil
        each do |dir, fn|
          fileInfo = nil
          case fn
          when @@cppFile
            fileInfo = createCompilationFileInfo(:cpp, :lib, File.expand_path(fn, dir))
            type = (type.nil? or type == "cpp") ? "cpp" : "mixed"
          when @@dFile
            fileInfo = createCompilationFileInfo(:d, :lib, File.expand_path(fn, dir))
            type = (type.nil? or type == "d") ? "d" : "mixed"
          end
          commands << CompileCommand.new(fileInfo, @@fileStore) if fileInfo
        end
        # Link them into a library
        dirLib = File.expand_path("lib", @base)
        archiveExtension = nil
        case operatingSystem
        when "Linux"
          archiveExtension = ".a"
        when Collection.new("MinGW", "Windows")
          archiveExtension = ".lib"
        end
        if File.exist?(dirLib)
          libName = File.expand_path("lib#{name}" + archiveExtension, dirLib)
          fileInfo = FileInfo.new(libName)
          fileInfo["libName"] = libName
          fileInfo["objects"] = commands.collect{|command|command.output}
          commands << ArchiveCommand.new(fileInfo, @@fileStore)
        else
          puts("WARNING::Directory \"#{dirLib}\" was not found, I will not create the library.")
        end
        # Copy the include files
        dirInclude = File.expand_path("include", @base)
        if File.exist?(dirInclude)
          each do |dir, fn|
            case fn
            when @@hppFile
              commands << CopyCommand.new(File.expand_path(fn, dir), dirInclude)
            end
          end
        else
          puts("WARNING::Directory \"#{dirInclude}\" was not found, I will not copy the include files.")
        end

      when Collection.from([@@cppFile, @@dFile])
        # Build test or main application
        type = @target[/\.([^\.]+)$/, 1].to_sym
        #  Compile the target
        fileInfo = createCompilationFileInfo(type, (unitTest? ? :unitTest : :lib), @target)
        allHeaders = fileInfo["internalHeaders"] + fileInfo["externalHeaders"]
        objects = [CompileCommand.new(fileInfo, @@fileStore)]
        #  Compile all the referenced modules
        fileInfo["internalHeaders"].each do |ih|
          im = ih
          im = im.gsub(/\.hpp$/, ".cpp") if type == :cpp
          if internalFile?(im) and fullFileName(im) != @target
            fi = createCompilationFileInfo(type, :lib, fullFileName(im))
            objects << CompileCommand.new(fi, @@fileStore)
            allHeaders += fi["internalHeaders"] + fi["externalHeaders"]
          end
        end
        commands += objects
        allHeaders.uniq!
        allHeaders.sort!
        #  Link all the object files
        linkInfo = FileInfo.new(execName)
        linkInfo["execName"] = execName
        linkInfo["linker"] = @@linkerPerType[type]
        linkInfo["objectFiles"] = objects.collect{|obj|obj.output}
        linkInfo["settings"] = linkSettings(type, allHeaders)
        commands << LinkCommand.new(linkInfo, @@fileStore)

      else
        raise "Target #{@target} is not supported"
      end

    else
      raise "Build command \"#{command}\" not supported"
    end
    commands
  end

  def formatCommands
    commands = []
    # Format all local files, do not recurse
    each(false) do |dir, fn|
      fileInfo = nil
      case fn
      when Collection.from([@@cppFile, @hppFile])
        commands << FormatCommand.new(File.expand_path(fn, dir), astyleStyle)
      when @@dFile
      end
    end
    commands
  end
  
  def unitTest?
    !@target.nil? && !(Collection.from(["test.cpp", "Test.d", "main.cpp", "Main.d"]) === File.basename(@target))
  end
  def name
    if !@settings.nil? and @settings.has_key?("name")
      @settings["name"]
    else
      File.basename(@base)
    end
  end
  def execName
    res = nil
    execExtension = nil
    case operatingSystem
    when "Linux"
      execExtension = ".exec"
    when "MinGW"
      execExtension = ".exe"
    when "Windows"
      execExtension = ".exe"
    end
    if !@settings.nil? and @settings.has_key?("name") and @@definingFiles.include?(@settings["name"])
      res = @settings["name"]
    else
      res = @target.gsub(/#{File.extname(@target)}$/, (unitTest? ? ".unit" : "") + execExtension)
    end
    return res
  end

  def createCompilationFileInfo(type, subType, fileName)
    fileInfo = nil

    # Create the settings based on the included files
    internalHeaders, externalHeaders, includeDirs = findIncludeFilesAndDirs(type, fileName)
    if $verbose
      puts("Internal headers: #{internalHeaders}")
      puts("External headers: #{externalHeaders}")
    end
    settings = [compilationSetting(type, "always")]
    (internalHeaders + externalHeaders).each do |incl|
      settings << compilationSetting(type, incl)
    end
    settings = settings.compact.uniq.sort

    # The objectName and type-specific settings and
    objectExtension = nil
    case operatingSystem
    when "Linux"
      objectExtension = ".o"
    when "MinGW"
      objectExtension = ".obj"
    when "Windows"
      objectExtension = ".obj"
    end
    case type
    when :cpp
      objectName = File.basename(fileName, ".cpp") + objectExtension
      case subType
      when :unitTest
        settings << "-DUNIT_TEST"
      end
    when :d
      objectName = File.basename(fileName, ".d") + objectExtension
      case subType
      when :unitTest
        settings << "-version=UnitTest"
      end
    end

    # Create the FileInfo object
    fileInfo =  FileInfo.new(objectName)
    fileInfo["settings"] = settings.join(" ")
    fileInfo["internalHeaders"] = internalHeaders
    fileInfo["externalHeaders"] = externalHeaders
    fileInfo["includeDirs"] = includeDirs
    fileInfo["date"] = File.new(fileName).mtime.to_s
    fileInfo["dates"] = internalHeaders.collect{|incl|File.new(fullFileName(incl)).mtime.to_s}
    fileInfo["sourceFile"] = fileName
    fileInfo["directory"] = File.dirname(fileName)
    fileInfo["type"] = type.to_s

    fileInfo
  end

  def compilationSetting(type, incl)
    if @settings
      begin
        @settings[type.to_s]["compilation"][incl]
      rescue
        nil
      end
    else
      @successor.compilationSetting(type, incl)
    end
  end

  def linkSettings(type, includes)
    settings = []
    (["always"] + includes).each do |incl|
      cs = linkSetting(type, incl)
      settings << cs if cs
    end
    settings = settings.uniq.sort
    return settings.join(" ")
  end
  def linkSetting(type, incl)
    if @settings
      @settings[type.to_s]["linking"][incl]
    else
      @successor.linkSetting(type, incl)
    end
  end

  def dirPerFile(file)
    res = nil
    relativeName = relativeFileName(file)
    fileName = File.basename(relativeName)
    relativeDir = File.dirname(relativeName)
    relativeDir = "" if relativeDir == "."
    if @dirsPerFile.has_key?(fileName)
      dirs = @dirsPerFile[fileName].dup
      ix = 0
      while ix < relativeDir.length
        ix += 1
        dirs.reject!{|dir|relativeDir[-ix] != dir[-ix]}
      end
      case dirs.length
      when 0
        # We will try @successor
      when 1
        res = dirs[0]
        len = res.length - relativeDir.length
        res = res[0, len]
      else
        raise "I found #{dirs.length} different files matching \"#{file}\"" if dirs.length != 1
      end
    end
    # Ask the successor if not found
    res = @successor.dirPerFile(file) if (res.nil? and @successor)
    res
  end
  def relativeFileName(file)
    relativeName = file.dup
    if !(Collection.from([@@cppFile, @@hppFile]) === file)
      relativeName.gsub!(".", "/")
      relativeName += ".d"
    end
    relativeName
  end
  def fullFileName(file)
    File.expand_path(relativeFileName(file), dirPerFile(file))
  end
  def internalFile?(file)
    dirPerFile(file)
  end

  def findIncludeFilesAndDirs(type, fn)
    puts("\nLooking for includes for #{fn}") if $verbose
    internalHeaders = {}
    externalHeaders = {}
    files2Check = Dependency.includedFiles(type, fn)
    while !files2Check.empty?
      files2Check.uniq!
      #       puts("I have #{files2Check.length} files to check")
      newFiles2Check = []
      files2Check.each do |file|
        if internalFile?(file)
          Dependency.includedFiles(type, fullFileName(file)).each do |header|
            newFiles2Check << header if (!internalHeaders[header] and !externalHeaders[header])
          end
          internalHeaders[file] = true
        else
          externalHeaders[file] = true
        end
      end
      files2Check = newFiles2Check
    end
    tmp = internalHeaders.keys.collect do |file|
      dirPerFile(file)
    end
    tmp = tmp.uniq.sort
    internalHeaders = internalHeaders.keys.sort
    externalHeaders = externalHeaders.keys.sort
    if $verbose
      puts("\tinternalHeaders = #{internalHeaders}")
      puts("\texternalHeaders = #{externalHeaders}")
      puts("\ttmp = #{tmp}")
    end
    return internalHeaders, externalHeaders, tmp
  end

  def excludedDirs
    if !@settings.nil? and @settings.has_key?("exclude")
      [File.expand_path(@settings["exclude"], @base)]
    else
      []
    end
  end

  def each(recurse = true)
    recursor = Proc.new do |dir|
      res = true
      @@definingFiles.each do |fn|
        res = false if File.exist?(File.expand_path(fn, dir))
      end
      res = false if excludedDirs.include?(dir)
      res = false if Collection.from(["lib", "include"]) === File.basename(dir)
      res
    end
    Dir.each(@base, recursor) do |dir, fn|
      case fn
      when Collection.from(@@wantedFiles)
          puts("#{dir} #{fn}")
        yield(dir, fn)
      end
      nil
    end
    # Iterate also over the files of the successor
    @successor.each{|dir, fn|yield(dir, fn)} if (recurse and !@successor.nil?)
  end

  def Tree.allTrees(base)
    trees = []
    Dir.eachDir(base) do |dir|
      Dir.chdir(dir) do
        Dir["*"].each do |fn|
          case fn
          when Collection.from(@@definingFiles)
            trees << Tree.create(dir)
          end
        end
      end
      nil
    end
    trees
  end

  def Tree.cleanFileStore
    @@fileStore.clean
  end

  def Tree.create(pwd, definingFiles = nil)
    definingFiles = @@definingFiles if definingFiles.nil?
    res = nil
    prevTree = nil
    settingsFile = nil
    target = File.file?(pwd) && File.expand_path(pwd)
    base = pwd
    foundRoot = false
    while !foundRoot
      base, files = Tree.findBaseFiles(base, definingFiles)
      files.each do |file|
        case file
        when "root.tree"
          foundRoot = true
          settingsFile = file
        when "disabled.tree"
          raise "You are inside a disabled tree"
        else
          target ||= File.expand_path(file, base)
        end
      end
      if res.nil?
        prevTree = res = Tree.new(base, settingsFile, target)
      else
        prevTree.successor = Tree.new(base, settingsFile, target)
        prevTree = prevTree.successor
      end
      base = File.dirname(base)
      target = nil
    end
    res
  end

  def Tree.findBaseFiles(pwd, definingFiles)
    base, files = nil, []
    # Recursively descend until at least one defining file has been found
    here = pwd
    while base.nil?
      # Try to find a match
      definingFiles.each do |lfile|
        if File.exist?(File.expand_path(lfile, here))
          base = here
          files << lfile
        end
      end
      break if !base.nil?
      # No match was found, decrease "here" if possible
      tmp = File.dirname(here)
      raise "Could not find any basefile from \"#{pwd}\" (possibilities: #{definingFiles})" if tmp == here
      here = tmp
    end
    return base, files
  end

  def pushURL
    if !@settings.nil? and @settings.has_key?("git")
      @settings["git"]["push"][ENV["USER"]]
    elsif @successor
      @successor.pushURL
    else
      raise "No git settings found"
    end
  end
  def pullURL
    if !@settings.nil? and @settings.has_key?("git")
      @settings["git"]["pull"][ENV["USER"]]
    elsif @successor
      @successor.pullURL
    else
      raise "No git settings found"
    end
  end
  def astyleStyle
    if !@settings.nil? and @settings.has_key?("astyle")
      @settings["astyle"]
    elsif @successor
      @successor.astyleStyle
    else
      # Default style
      "gnu"
    end
  end
  def loadSettings
    if @settingsFile == "root.tree"
      fnSettings = File.expand_path(@settingsFile, @base)
      @settings = YAML::load(File.open(fnSettings))
      raise "ERROR::Empty settings file found." if !@settings
      if @settings["include"]
        fnExtraSettings = File.expand_path(@settings["include"], @base)
        @settings.merge!(YAML::load(File.open(fnExtraSettings))) do |k, ov, nv|
          raise "Duplicate settings found in \"#{fnExtraSettings}\" and \"#{fnSettings}\"."
        end
      end
      if @settings["cpp"]
        @settings["cpp"]["compilation"] = {} if @settings["cpp"]["compilation"].nil?
        @settings["cpp"]["linking"] = {} if @settings["cpp"]["linking"].nil?
      end
      if @settings["c"]
        @settings["c"]["compilation"] = {} if @settings["c"]["compilation"].nil?
        @settings["c"]["linking"] = {} if @settings["c"]["linking"].nil?
      end
      if @settings["d"]
        @settings["d"]["compilation"] = {} if @settings["d"]["compilation"].nil?
        @settings["d"]["linking"] = {} if @settings["d"]["linking"].nil?
      end
    else
      @settings = nil
    end
  end

  def Tree.createDefaultRootFile
    File.open("root.tree", "w") do |fo|
      fo.puts(%q@#Uncomment lines you want to use, filling in the <>'s
#name: <name>
d:
 compilation:
#  always: -I<path-to-include>
 linking:
#  always: -L-l<lib-name> -L-L<path-to-libs>
@)
    end
  end
end

if __FILE__ == $0
  tree = Tree.new(Dir.pwd)
end
