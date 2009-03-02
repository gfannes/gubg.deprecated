# The D language parser for gb

require("tools")

class DLanguage
  attr :localIncludes, true
  attr :objectDir, true
  def initialize(baseFile)
    # Find and load the base file (e.g., base.rb)
    pwd = Dir.pwd
    begin
      bContinue = true
      begin
        if File.exist?(baseFile) or File.exist?(baseFile+'.rb')
          @baseDir = Dir.pwd
          @baseFile = baseFile
          @basePath = File.expand_path(@baseFile, @baseDir)
          require('./'+baseFile)
          bContinue = false
        else
          prevPwd = Dir.pwd
          Dir.chdir('..')
          if prevPwd == Dir.pwd
            raise "ERROR::Could not find #{baseFile} in any of the lower dirs"
          end
        end    
      end while bContinue
    ensure
      # Make sure we do not change the path
      Dir.chdir(pwd)
    end

    load(@basePath)
    @localSources = []
    @localSources += $localSources if $localSources
    @localIncludes = ([@baseDir]+@localSources).collect{|dir|"-I#{dir}"}.join(' ')

#     @objectDir = tempDir
    @objectDir = '/tmp/d-objects'
    Dir.mkdir(@objectDir) if !File.exist?(@objectDir)
    @mainObjectDir = '/tmp/d-mains'
    Dir.mkdir(@mainObjectDir) if !File.exist?(@mainObjectDir)
    
    CompilationUnit.setEnvironment(self)

  end

  def clean
#     Dir.chdir(@objectDir) do
#       system("rm *.o")
#     end
#     Dir.rmdir(@objectDir)
  end

  # Collect information on all local and external modules imported from sourceFile
  def collectDependencyInformation(sourceFile)
    relDir = Dir.relative(Dir.pwd, @baseDir)
    case relDir
    when "./"
      sourceImport = "#{File.basename(sourceFile, '.d')}"
    else
      sourceImport = relDir + "/#{File.basename(sourceFile, '.d')}"
    end
    sourceImport.gsub!('/', '.')

    allCompilationUnits = {}

    sourceBuild = CompilationUnit.new(sourceImport, :build)
    allCompilationUnits[sourceImport] = sourceBuild

    allLocalImports = []
    localImports = sourceBuild.localImports
    while !localImports.empty?
      newImports = []
      allLocalImports += localImports
      allLocalImports.uniq!
      localImports.each do |import|
        if !allCompilationUnits.has_key?(import)
          cu = CompilationUnit.new(import, :compile)
          allCompilationUnits[import] = cu
          newImports += (cu.localImports - allLocalImports)
        end
      end
      localImports = newImports
    end

    return allCompilationUnits
  end

  # Compile all locally imported files
  def compile(dependencyInformation)
    objectFiles = []

    # Compile the compilation units tagged as :compile
    dependencyInformation.keys.sort.each do |import|
      cu = dependencyInformation[import]
      if cu.operation == :compile
        objectFiles << cu.compile
        return nil if objectFiles.last.nil?
      end
    end

    return objectFiles
  end

  # Build the specified source file
  def build(sourceFile, objectFiles)
    executableFile = 'execTest_'+sourceFile.gsub(/\.d$/, '')
    command = "dmd -of#{executableFile} #{sourceFile} -od#{@mainObjectDir} -version=Test #{@localIncludes} " + objectFiles.join(' ')
    puts("LINKING:\t#{command}")
    if system(command)
      return executableFile
    else
      return nil
    end
  end

  # Extract all imported modules and filter according to local and external imports
  # This is a very basic parsing of D files and does not care with version() statements etc.
  def extractImports(sourceFile)
    # Extract all imported modules
    imports = []
    re = /^(private|public)? *import +(.+);.*$/
    String.loadLines(sourceFile).each do |line|
      if md = re.match(line)
        imports << md[2]
      end
    end

    # Filter 
    localImports, externalImports = [], []
    imports.each do |import|
      if localImport2Path(import)
        localImports << import
      else
        externalImports << import
      end
    end
    
    return localImports, externalImports
  end

  # Convert a local import to its absolute path
  def localImport2Path(import)
    path = File.expand_path(import.gsub('.','/') + '.d', @baseDir)
    if File.exist?(path)
      return path
    else
      @localSources.each do |dir|
        path = File.expand_path(import.gsub('.','/') + '.d', dir)
        if File.exist?(path)
          return path
        end
      end
    end
    return nil
  end
end

class CompilationUnit
  attr :localImports, true
  attr :operation, true
  def initialize(importName, operation)
    @importName, @operation = importName, operation
    @sourceFile = @@env.localImport2Path(@importName)
    @localImports, @externalImports = @@env.extractImports(@sourceFile)
    @objectFile = File.expand_path(@importName + '.o', @@env.objectDir)
  end

  def CompilationUnit.setEnvironment(env)
    @@env = env
  end

  def compile
    res = nil
    case @operation
    when :build
    when :compile
      if !File.exist?(@objectFile) || (File.mtime(@objectFile) < File.mtime(@sourceFile))# || true
        command = "dmd -c -of#{@objectFile} #{@sourceFile} #{@@env.localIncludes}"
        puts("COMPILATION:\t#{command}")
        if !system(command)
          puts("\nERROR::Compilation of #{@sourceFile} failed")
        else
          res = @objectFile
        end
      else
        puts("Object file #{@objectFile} is sufficiently recent.")
        res = @objectFile
      end
    else
      puts("ERROR::Unknown operation #{@operation}")
    end
    res
  end
end
