require("tools/math.rb")

def operatingSystem
  case RUBY_PLATFORM
  when /64-linux/i
    "Linux64"
  when /win/i
    "Windows"
  when /ming/i
    "MinGW"
  when /ix/i,/ux/i,/gnu/i
    "Linux"
  else
    "Other"
  end
end
def mingw?
  operatingSystem == "MinGW"
end

def emacs
  case operatingSystem
  when "MinGW"
    "//damm/apps/emacs/emacs-21.3/bin/emacs"
  else
    "emacs"
  end
end

def which(command)
  paths = ENV["PATH"].split(":")
  case operatingSystem
  when "MinGW"
    command = command + ".exe" if !command[/\.exe$/]
    raise "ERROR::You have to set the environment vairable $MINGW_PATH to c:\\..." if !ENV["MINGW_PATH"]
    paths.unshift(ENV["MINGW_PATH"])
  end
  paths.each do |path|
    fileName = File.expand_path(command, path)
    if File.exist?(fileName)
      return fileName
    end
  end
  return nil
end

$gppCommand = nil
def gpp
  return $gppCommand ||= which('g++')
end

class Array
  #the result of this function is six, an array of sort indices
  #e.g., self[six[0]] is the smallest value
  def sortIndex
    d=[]
    self.each_with_index {|x,i| d[i]=[x,i]}
    if block_given?
      d.sort{|x,y| yield(x[0],y[0])}.collect {|x| x[1]}
    else
      d.sort.collect{|x| x[1]}
    end
  end
  def sortReverseIndex
    d=[]
    self.each_with_index {|x,i| d[i]=[x,i]}
    if block_given?
      d.sort{|x,y| yield(y[0],x[0])}.collect {|x| x[1]}
    else
      d.sort{|x,y|y[0]<=>x[0]}.collect{|x| x[1]}
    end
  end
  def sortBy(ord=[])
    raise "ERROR::Lengths of the array and order array are different (#{self.length} != #{ord.length})." if ord.length!=self.length
    self.values_at(*ord)
  end
  def sortBy!(ord=[])
    raise "ERROR::Lengths of the array and order array are different (#{ord.length}!=#{self.length})." if ord.length!=self.length
    temp=values_at(*ord)
    (0...length).each do |ix|
      self[ix]=temp[ix]
    end
    return self
  end
  def Array.load(fileName)
    String.loadLines(fileName).collect{|str|str.interpret}
  end
  def export(fileName)
    File.open(fileName,"w") do |fo|
      each do |el|
        fo.puts(el) 
      end
    end
  end
  alias index_old index
  def index(arg=nil)
    if block_given?
      ix=-1
      each do |el|
        ix+=1
        return ix if yield(el)
      end
      return nil
    else
      return index_old(arg)
    end
  end
  def maxIndex
    if block_given?
      m,mix=nil,nil
      each_with_index do |el,ix|
        v=yield(el)
        if m.nil? or v>m
          mix=ix
          m=v
        end
      end
      mix
    else
      m,mix=nil,nil
      each_with_index do |v,ix|
        if m.nil? or v>m
          mix=ix
          m=v
        end
      end
      mix
    end
  end
  def minIndex
    if block_given?
      m,mix=nil,nil
      each_with_index do |el,ix|
        v=yield(el)
        if m.nil? or v<m
          mix=ix
          m=v
        end
      end
      mix
    else
      m,mix=nil,nil
      each_with_index do |v,ix|
        if m.nil? or v<m
          mix=ix
          m=v
        end
      end
      mix
    end
  end
  def to_s
    "["+join('; ')+"]"
  end
  def collect_with_index
    ix = -1
    collect do |el|
      ix += 1
      yield(el, ix)
    end
  end
end

class Dir
  # Descends recursively in each subdir and calls the block, passing it each directory and filename it finds
  # recursor can be a Proc that indicates with true in which subdirs should be recursed
  def Dir.each(startDir="./", recursor=nil, permissionDenied=nil, level=0)
    raise "ERROR::I expect a block for eachFile" if !block_given?
    subDirs=[]
    begin
      startDir=File.expand_path(startDir) if level==0
      Dir.foreach(startDir) do |entry|
        begin
          fnDir=File.expand_path(entry,startDir)
          if fnDir.length>startDir.length
            if File.file?(fnDir)
              if File.readable?(fnDir)
                return if yield(startDir.dup,entry.dup)
              else
                permissionDenied << fnDir
              end
            elsif File.directory?(fnDir)
              subDirs << fnDir if !(recursor and !recursor.call(fnDir))
            end
          end
        rescue ArgumentError
          puts("WARNING::Skipping file \"#{entry}\" in \"#{startDir}\" because it does not seem to exist.")
        end
      end
      subDirs.each do |subDir|
        each(subDir,recursor,permissionDenied,level+1){|base,fn|return if yield(base,fn)}
      end
    rescue Errno::EACCES
      permissionDenied << startDir if permissionDenied
    end
  end
  # Descends in each subdir recursively and calls the block passing the directory
  # recursor can be a Proc that indicates with true in which subdirs should be recursed
  def Dir.eachDir(startDir="./", recursor=nil, permissionDenied=nil, level=0)
    raise "ERROR::I expect a block for eachFile" if !block_given?
    subDirs=[]
    begin
      startDir=File.expand_path(startDir) if level==0
      return if yield(startDir)
      if File.exist?(startDir)
        Dir.foreach(startDir) do |entry|
          fnDir=File.expand_path(entry,startDir)
          if fnDir.length>startDir.length and File.directory?(fnDir)
            subDirs << fnDir if !(recursor and !recursor.call(fnDir))
          end
        end
        subDirs.each do |subDir|
          eachDir(subDir,recursor,permissionDenied,level+1){|dir|return if yield(dir)}
        end
      end
    rescue Errno::EACCES
      permissionDenied << startDir if permissionDenied
    end
  end
  def Dir.mkchdir(dir, &block)
    Dir.mkdir(dir) if !File.exist?(dir)
    Dir.chdir(dir,&block)
  end
  def Dir.relative(dir, pwd = nil)
    pwd ||= Dir.pwd
#    puts("dir = #{dir}")
#    puts("pwd = #{pwd}")
    if dir[/^\//] or dir[/^[a-zA-Z]:\//]
      dir = dir.dup
      if dir[pwd+'/']
#        puts("Subpath")
        dir[pwd+'/'] = ''
        dir = './' if dir == ''
      elsif dir == pwd
#        puts("Equal")
        dir = './'
      else
#        puts("Different")
        parts = pwd.split('/')
#        puts("parts = #{parts}")
        parts.shift
#        puts("parts = #{parts}")
        ix = nil
        parts.each_with_index do |part,ix|
          startDir = '/'+parts[0,ix+1].join('/')
#          puts("startDir = #{startDir}")
          break if !dir[/^#{startDir}/]
        end
#        puts("dir.split = #{dir.split('/')}")
        nr = parts.length - ix
#        puts("nr = #{nr}, ix = #{ix}")
        if ix == 0
          dir['/'] = '../'*nr
        else
          dir['/'+parts[0,ix].join('/')+'/'] = '../'*nr
        end
      end
    end
    dir
  end
end

class String
  def String.load(fileName)
    IO.read(fileName)||""
  end
  def String.loadLines(fileName)
    String.load(fileName).split("\n")
  end
  def export(fileName)
    File.open(fileName,"wb") do |fo|
      fo.print(self)
    end
  end

  @@floatPattern = /^[+-]?(\d[\d_]*)?[,\.][\d_]*(e[+-]?)?(_*\d[\d_]*)$/i
  @@integerPattern = /^[+-]?[0-9][\d_]*$/
  @@zeroPattern = /^0\.?0?$/
  def interpret
    case self
    when nil
      nil
    when @@zeroPattern
      0
    when @@integerPattern
      self.to_i
    when @@floatPattern
      self.to_f
    else
      self
    end
  end
end

class Hash
  def to_s
    res="{"
    first=true
    each do |k,v|
      if first
        first=false
      else
        res += ", "
      end
      res += "#{k} => #{v}"
    end
    res += "}"
    res
  end
  # is returnZero == true, this method will return a Hash containing 0 elements, instead of nil if the sum is 0
  def fractions(returnZero = false)
    res=Hash.new(0)
    s=values.sum.to_f
    if s>0
      each do |k,v|
        res[k]=v/s
      end
    else
      puts("WARNING::Sum of elements is zero while computing the fractions")
      if returnZero
        each do |k,v|
          res[k]=0
        end
      else
        res = nil
      end
    end
    res
  end
  def eachDec(&block)
    ks,vs=keys,values
    six = vs.sortReverseIndex
    ks.sortBy(six).zip(vs.sortBy(six)).each do |k,v|
      block.call(k,v)
    end
  end
  def eachInc(&block)
    ks,vs=keys,values
    six = values.sortIndex
    ks.sortBy(six).zip(vs.sortBy(six)).each do |k,v|
      block.call(k,v)
    end
  end
end

#A Collection enables case-when statements to be compared with a collection of things that can fit. If any of the collection arguments matches (with ===), the collection matches
class Collection < Array
  def initialize(*elements)
    @elements = elements
  end
  def ===(rhs)
    @elements.any?{|el|el===rhs}
  end
end

$timingLevel = 0
$maxTimingLevel = 0
def time(str, newline = false)
  raise "You have to provide a block that contains the code that should be timed" if !block_given?
  if $maxTimingLevel == $timingLevel
    print("#{'  '*$timingLevel}#{str} ... ")
    print("\n") if newline
  else
    $maxTimingLevel = $timingLevel
    print("\n#{'  '*$timingLevel}#{str} ... ")
    print("\n") if newline
  end
  $stdout.flush
  $timingLevel += 1
  startTime=Time.now
  res = yield
  stopTime=Time.now
  timeStr="%.3f sec"%(stopTime-startTime)
  $timingLevel -= 1
  if $maxTimingLevel == $timingLevel
    # We did not go deeper recursively, close without enter
    puts("finished (#{timeStr})")
  else
    # We went deeper recursively
    $maxTimingLevel = $timingLevel
    puts("#{'  '*$timingLevel}finished (#{timeStr})")
  end
  res
end

# Temporary path and files
def tempDir(dir='/tmp')
  res = nil
  begin
    res = File.expand_path("tempDir-#{rand(1000000000)}.tmp", dir)
  end while File.exist?(res)
  Dir.mkdir(res)
  res
end
def tempPath(dir='/tmp')
  res=nil
  if %w[Linux Linux64].include?(operatingSystem)
    dir[/\/$/]='' if dir[dir.length-1]==47
    begin
      res=dir+"/tempPath-#{rand(1000000000)}.tmp"
    end while File.exist?(res)
  else
    res="tempPath-#{rand(1000000000)}.tmp"
  end
  res
end
def tempFile(extension)
  File.basename(tempPath)+'.'+extension.gsub(/^\./,'')
end

def md5sum(fileName)
  require("digest/md5")
  return Digest::MD5.hexdigest(String.load(fileName))
end

class Collection < Array
  def ===(rhs)
    any?{|el|el === rhs}
  end
  def Collection.from(ary)
    collection=Collection.new
    ary.each do |el|
      collection << el
    end
    collection
  end
end

if __FILE__==$0
  puts Dir.relative(Dir.pwd+'/')
end
