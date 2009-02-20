#!/usr/bin/env ruby
require("tools")

def includeFiles(fileName)
  incl=String.load(fileName).split("\n").select{|line|line[/^\#include /]}
  incl.collect! do |line|
    case line
    when /^\#include \"(.+)\"/
        line[/^\#include \"(.+)\"/,1]
    when /^\#include <(.+)>/
        line[/^\#include <(.+)>/,1]
    else
      puts("WARNING::Some problem occured")
      nil
    end
  end
  return incl.compact
end

def recursiveIncludeFiles(fileName,res)
  incl=[]
  news=includeFiles(fileName)
  while !news.empty?
    newNews=news.collect{|n|res[:includesPerInclude][n]}.flatten.compact.uniq
    incl+=news
    incl.compact!
    incl.uniq!
    news=newNews-incl
  end
  return incl
end

# This function collects build information
# It tries to locate baseBuild.rb and starts its search from there
# The path is not altered at exit
def collectBuildInfo
  res={
    :baseDir => nil,
    :headerFiles => nil,
    :headerArgs => nil,
    :sourceFiles => nil,
    :objectFiles => nil,
    :objectArgs => nil,
    :testFiles => nil,
    :testExecFiles => nil,
    :compileOptionsPerInclude => nil,
    :buildOptionsPerInclude => nil,
    :includesPerInclude => nil,
    :objectPerInclude => nil,
#    :configPerDir => nil,
  }

#  fnConfig = 'config.rb'
  pwd=Dir.pwd
  $compileOptionsPerInclude = {}
  $buildOptionsPerInclude = {}
  begin
    # find and require buildBase.rb
    name="buildBase.rb"
    Dir.chdir('source') if File.exist?('source')
    bContinue=true
    begin
      if File.exist?(name) or File.exist?(name+'.rb')
        puts("Found #{name} at level #{Dir.pwd}")
        require('./'+name)
        bContinue=false
      else
        prevPwd=Dir.pwd
        Dir.chdir('..')
        if prevPwd==Dir.pwd
          raise "ERROR::Could not find #{name} in any of the lower dirs"
        end
      end    
    end while bContinue
    res[:baseDir] = Dir.pwd

    # collect the source, header and test files
    headerFiles,sourceFiles,objectFiles,testFiles,testExecFiles=[],[],[],[],[]
    includesPerInclude = {}
#    configPerDir=Hash.new{|h,k|h[k]={}}
#    configH=nil
    Dir.each('./') do |dir,fn|
#       if configPerDir.has_key?(dir)
#         puts("Found in cache")
#         configH=configPerDir[dir]
#       else
#         puts("Dirname of #{dir} == #{File.dirname(dir)}")
#         configH=configPerDir[File.dirname(dir)]
#         puts("\tConfig = #{configH.toS}")
#         if File.exist?(fnC=File.expand_path(fnConfig,dir))
#           puts("Found config in #{dir}")
#           load(fnC)
#           configH.merge!(config())
#         end
#         puts("\tSetting #{dir} to #{configH.toS}")
#         configPerDir[dir]=configH.dup
#       end
#       puts("Config is now #{configH.toS}")

      case fn
      when /^test[^\/]*\.cpp$/
        testFiles << File.expand_path(fn,dir)
        testExecFiles << File.expand_path(fn,dir).gsub(/test([^\/]*)\.cpp$/,'execTest\1')
      when /\.cpp$/
        sourceFiles << File.expand_path(fn,dir)
        objectFiles << File.expand_path(fn,dir).gsub(/\.cpp$/,'.o')
      when /\.hpp$/
        headerFiles << (fnTmp=File.expand_path(fn,dir))
        includesPerInclude[fn]=includeFiles(fnTmp)
      else
      end
      nil
    end
    res[:headerFiles] = headerFiles
    res[:sourceFiles] = sourceFiles
    res[:objectFiles] = objectFiles
    res[:testFiles] = testFiles
    res[:testExecFiles] = testExecFiles
    res[:compileOptionsPerInclude] = $compileOptionsPerInclude
    res[:buildOptionsPerInclude] = $buildOptionsPerInclude
    res[:includesPerInclude] = includesPerInclude
#    puts includesPerInclude.toS

    # create the header compilation arguments
    res[:headerArgs] = headerFiles.collect{|headerFile|File.dirname(headerFile)}.uniq.collect{|headerDir|"-I#{headerDir}"}.join(' ')

    # create the object compilation arguments
    res[:objectArgs] = objectFiles.join(' ')
    res[:objectPerInclude] = {}
    objectFiles.each do |objFile|
      res[:objectPerInclude][File.basename(objFile.gsub(/\.o$/,".hpp"))]=objFile
    end

  ensure
    # Make sure we do not change the path
    Dir.chdir(pwd)
  end
  return res
end

def compileFile(object,source,headers,res)
  extraCompileOptions = recursiveIncludeFiles(source,res).collect{|includeFile|res[:compileOptionsPerInclude][includeFile]}.uniq.compact.join(' ')
  command="g++ -g -c -o #{object} #{source} #{headers} #{extraCompileOptions}"
  puts command
  raise "ERROR" if !system(command)
end

def buildFile(exec,file,objects,headers,res)
  allIncludes = recursiveIncludeFiles(file,res)
  extraCompileOptions = allIncludes.collect{|includeFile|res[:compileOptionsPerInclude][includeFile]}.uniq.compact.join(' ')
  extraBuildOptions = allIncludes.collect{|includeFile|res[:buildOptionsPerInclude][includeFile]}.uniq.compact.join(' ')
#  command="g++ -g -o #{exec} #{file} #{objects} #{headers} #{extraCompileOptions} #{extraBuildOptions}"
  objects=allIncludes.collect{|incl|res[:objectPerInclude][incl]}.compact.uniq.join(' ')
  command="g++ -g -o #{exec} #{file} #{headers} #{objects} #{extraCompileOptions} #{extraBuildOptions}"
  puts command
  raise "ERROR" if !system(command)
end

# compiles all local source files to objects and then compiles tests to executables
def buildLocal(res)
  pwd=Dir.pwd
  # compile all the source files to objects
  sourceFiles=res[:sourceFiles].select{|fn|fn[/^#{pwd}/]}
  objectFiles=res[:objectFiles].select{|fn|fn[/^#{pwd}/]}
  sourceFiles.zip(objectFiles).each do |sourceFile,objectFile|
    dir,fnSource,fnObject=File.dirname(sourceFile),File.basename(sourceFile),File.basename(objectFile)
    Dir.chdir(dir) do
      compileFile(fnObject,fnSource,res[:headerArgs],res)
    end
  end
  
  #compile all the test files to executables
  testFiles=res[:testFiles].select{|fn|fn[/^#{pwd}/]}
  testExecFiles=res[:testExecFiles].select{|fn|fn[/^#{pwd}/]}
  testFiles.zip(testExecFiles).each do |testFile,execFile|
    dir,fnTest,fnExec=File.dirname(testFile),File.basename(testFile),File.basename(execFile)
    Dir.chdir(dir) do
      buildFile(fnExec,fnTest,res[:objectArgs],res[:headerArgs],res)
    end
  end
end

# compiles all source files to objects and then compiles tests to executables
def buildAllSource(res)
  # compile all the source files to objects
  sourceFiles=res[:sourceFiles]
  objectFiles=res[:objectFiles]
  sourceFiles.zip(objectFiles).each do |sourceFile,objectFile|
    dir,fnSource,fnObject=File.dirname(sourceFile),File.basename(sourceFile),File.basename(objectFile)
    Dir.chdir(dir) do
      puts("\n***Building objects in \"#{dir}\"***")
      compileFile(fnObject,fnSource,res[:headerArgs],res)
    end
  end
end

def buildAllTests(res)
  #compile all the test files to executables
  testFiles=res[:testFiles]
  testExecFiles=res[:testExecFiles]
  testFiles.zip(testExecFiles).each do |testFile,execFile|
    dir,fnTest,fnExec=File.dirname(testFile),File.basename(testFile),File.basename(execFile)
    Dir.chdir(dir) do
      puts("\n***Building tests in \"#{dir}\"***")
      buildFile(fnExec,fnTest,res[:objectArgs],res[:headerArgs],res)
    end
  end
end

# expands all templates
def expandAll(res)
  (res[:sourceFiles]+res[:headerFiles]+res[:testFiles]).each do |fileName|
    dir,fn=File.dirname(fileName),File.basename(fileName)
    Dir.chdir(dir) do
      puts("\n***Expanding templates in \"#{dir}\"***")
      command="expandTemplate #{fn}"
      puts command
      raise "ERROR" if !system(command)
    end
  end
end

# expands local templates
def expandLocal(res)
  pwd=Dir.pwd
  files=(res[:sourceFiles]+res[:headerFiles]+res[:testFiles]).select{|fn|fn[/^#{pwd}/]}
  files.each do |fileName|
    dir,fn=File.dirname(fileName),File.basename(fileName)
    Dir.chdir(dir) do
      puts("\n***Expanding templates in \"#{dir}\"***")
      command="expandTemplate #{fn}"
      puts command
      raise "ERROR" if !system(command)
    end
  end
end

# executes the local tests
def testLocal(res)
  pwd=Dir.pwd
  testExecFiles=res[:testExecFiles].select{|fn|fn[/^#{pwd}/]}
  failedTests=[]
  if testExecFiles.length > 1
    puts("\n\nMore than one local test found")
    testExecFiles.each do |exec|
      puts("\t#{exec}")
    end
    puts("Please run the required test yourself\n\n\n")
  else
    testExecFiles.each do |exec|
      dir,fnExec=File.dirname(exec),File.basename(exec)
      Dir.chdir(dir) do
        failedTests << exec if !system("./#{fnExec}")
      end
    end
  end
  if failedTests.empty?
    puts("SUCCESS::All local tests in #{pwd} are passing")
  else
    puts("FAILURE::Following tests in #{pwd} are NOT passing:")
    puts failedTests
  end
end

# executes all tests
def testAll(res)
  testExecFiles=res[:testExecFiles]
  failedTests=[]
  testExecFiles.each do |exec|
    dir,fnExec=File.dirname(exec),File.basename(exec)
    puts("\n***Testing in #{dir}***")
    Dir.chdir(dir) do
      failedTests << exec if !system("./#{fnExec}")
    end
  end
  puts("")
  if failedTests.empty?
    puts("SUCCESS::All local tests are passing")
  else
    puts("FAILURE::Following tests are NOT passing:")
    puts failedTests
  end
end
