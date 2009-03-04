require("yaml")

require("patterns/chainOfResponsibility")
require("tools/utils")
require("tools/filestore")

class Tree# < IChainOfResponsibility
  attr :rootTree, true
  attr :base, true
  attr :file, true
  attr :successor, true
  @@definingFiles = %w[root.tree disabled.tree main.cpp main.d test.cpp test.d test.rb]
  @@cppFile = /\.cpp$/
  @@hppFile = /\.hpp$/
  @@dFile = /\.d$/
  @@wantedFiles = [@@cppFile, @@hppFile, @dFile]
  def initialize(base, file)
    @base, @file = base, file
    loadSettings
    @dirPerFile = {}
    each do |dir, fn|
      raise "File \"#{fn}\" is present multiple times (\"#{@dirPerFile[fn]}\" and \"#{dir}\")" if @dirPerFile.has_key?(fn)
      @dirPerFile[fn] = dir
    end
    @fileStore = FileStore.new(File.expand_path(".obj", @base))
  end

  def buildCommands(command)
    commands = []
    case command
    when NilClass
      case @file

      when "root.tree"
        # Build all
        each do |dir, fn|
          fileInfo = nil
          case fn
          when @@cppFile
            fileInfo = createCompilationFileInfo(:cpp, :lib, dir, fn)
          when @@dFile
            fileInfo = createCompilationFileInfo(:d, :lib, dir, fn)
          end
          commands << CompileCommand.new(fileInfo, @fileStore) if fileInfo
        end

      when Collection.from(["test.cpp", "main.cpp"])
        objects = []
        internalHeaders, externalHeaders, includeDirs = findIncludeFilesAndDirs(@file)
        # Compile @file
        source = File.expand_path(@file, @base)
        objects << CPPCompileCommand.new(@base, source, includeDirs, cppCompileSettings(:lib, internalHeaders + externalHeaders))
        # Compile all the referenced modules
        internalHeaders.each do |ih|
          im = ih.gsub(/\.hpp$/, ".cpp")
          if dirPerFile(im)
            source = File.expand_path(im, dirPerFile(im))
            objects << CPPCompileCommand.new(@base, source, includeDirs, cppCompileSettings(:lib, internalHeaders + externalHeaders))
          end
        end
        commands += objects
        # Link all the object files
        exec = File.expand_path(name + ".bin", @base)
        commands << LinkCommand.new(exec, objects.collect{|obj|obj.output}, linkSettings(internalHeaders + externalHeaders))
      else
        raise "Not supported"
      end
    else
      raise "Not supported"
    end
    commands
  end
  def name
    File.basename(@base)
  end

  def createCompilationFileInfo(type, subType, dir, fn)
    fileInfo = nil

    # Create the settings based on the included files
    internalHeaders, externalHeaders, includeDirs = findIncludeFilesAndDirs(type, fn)
    settings = []
    (internalHeaders + externalHeaders).each do |incl|
      cs = compilationSetting(type, incl)
      settings << cs if cs
    end
    settings = settings.uniq.sort

    # The objectName  and type-specific settings and 
    case type
    when :cpp
      objectName = File.basename(fn, ".cpp") + ".o"
      case subType
      when :unitTest
        settings << "-DUNIT_TEST"
      end
    when :d
      objectName = File.basename(fn, ".d") + ".o"
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
    fileInfo["dates"] = internalHeaders.collect{|incl|File.new(fullFileName(incl)).mtime.to_s}
    fileInfo["sourceFile"] = File.expand_path(fn, dir)
    fileInfo["directory"] = dir
    fileInfo["type"] = type.to_s

    fileInfo
  end

  def compilationSetting(type, incl)
    if @settings
      @settings[type.to_s]["compilation"][incl]
    else
      @successor.compilationSetting(type, incl)
    end
  end

  def linkSettings(includes)
    settings = []
    includes.each do |incl|
      cs = linkSetting(incl)
      settings << cs if cs
    end
    settings = settings.uniq.sort
    return settings.join(" ")
  end
  def linkSetting(incl)
    if @settings
      @settings["cpp"]["linking"][incl]
    else
      @successor.linkSetting(incl)
    end
  end

  def dirPerFile(file)
    if @dirPerFile.has_key?(file)
      @dirPerFile[file]
    else
      if @successor
        @successor.dirPerFile(file)
      else
        nil
      end
    end
  end
  def fullFileName(file)
    File.expand_path(file, dirPerFile(file))
  end

  def findIncludeFilesAndDirs(type, fn)
    internalHeaders = {}
    externalHeaders = {}
    files2Check = [fn]
    while !files2Check.empty?
      newFiles2Check = []
      files2Check.each do |file|
        if dirPerFile(file)
          Dependency.includedFiles(type, File.expand_path(file, dirPerFile(file))).each do |header|
            newFiles2Check << header if (!internalHeaders[header] and !externalHeaders[header])
          end
          internalHeaders[file] = true
        else
          externalHeaders[file] = true
        end
      end
      files2Check = newFiles2Check
    end
    internalHeaders.delete(fn)
    return internalHeaders.keys.sort, externalHeaders.keys.sort, internalHeaders.keys.collect{|file|dirPerFile(file)}.uniq.sort
  end

  def each
    recursor = Proc.new do |dir|
      res = true
      @@definingFiles.each do |fn|
        res = false if File.exist?(File.expand_path(fn, dir))
      end
      res
    end
    Dir.each(@base, recursor) do |dir, fn|
      case fn
      when Collection.from(@@wantedFiles)
        yield(dir, fn)
      end
      nil
    end
    # Iterate also over the files of the successor
    @successor.each{|dir, fn|yield(dir, fn)} if !@successor.nil?
  end

  def baseFile
    File.expand_path(@file, @base)
  end

  def Tree.create(pwd, definingFiles = nil)
    definingFiles = @@definingFiles if definingFiles.nil?
    res = nil
    prevTree = nil
    base = pwd
    while true
      base, file = Tree.findBaseFile(base, definingFiles)
      if res.nil?
        prevTree = res = Tree.new(base, file)
      else
        prevTree.successor = Tree.new(base, file)
        prevTree = prevTree.successor
      end
      case file
      when "root.tree"
        break
      when "disabled.tree"
        raise "You are inside a disabled tree"
      else
      end
      base = File.dirname(base)
    end
    res
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
      # No match was found, decrease "here" if possible
      tmp = File.dirname(here)
      raise "Could not find any basefile from \"#{pwd}\" (possibilities: #{files})" if tmp == here
      here = tmp
    end
    return base, file
  end

  def pushURL
    if !@settings.nil?
      @settings["git"]["push"][ENV["USER"]]
    else
      @successor.pushURL
    end
  end
  def pullURL
    if !@settings.nil?
      @settings["git"]["pull"][ENV["USER"]]
    else
      @successor.pullURL
    end
  end
  def loadSettings
    if @file == "root.tree"
      fnSettings = File.expand_path(@file, @base)
      @settings = YAML::load(File.open(fnSettings))
      if @settings["include"]
        fnExtraSettings = File.expand_path(@settings["include"], @base)
        @settings.merge!(YAML::load(File.open(fnExtraSettings))) do |k, ov, nv|
          raise "Duplicate settings found in \"#{fnExtraSettings}\" and \"#{fnSettings}\"."
        end
      end
    else
      @settings = nil
    end
  end
end

if __FILE__ == $0
  tree = Tree.new(Dir.pwd)
end
