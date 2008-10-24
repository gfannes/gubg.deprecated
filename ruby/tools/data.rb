require("utils.rb")

class Dataset
  attr(:fields, true)
  attr(:delimiter, true)
  def initialize(fieldNames,delimiter = "\t")
    @fieldNames = fieldNames.dup
    @fields = @fieldNames.collect{[]}
    @delimiter=delimiter
  end
  def Dataset.load(fileName, delimiter="\t")
    lines=String.loadLines(fileName)
    data=Dataset.new(lines.shift.split(delimiter,-1),delimiter)
    lines.each do |line|
      data << line.split(delimiter,-1).collect{|str|str.interpret}
    end
    return data
  end
  def fieldNames
    @fieldNames.dup
  end

  def to_s
    res=@fieldNames.join(@delimiter)
    each do |row|
      res += "\n"+row.join(@delimiter)
    end
    res
  end
  def save(fileName, delimiter=nil)
    delimiter ||= @delimiter
    File.open(fileName,"w") do |fo|
      fo.puts(@fieldNames.join(delimiter))
      each do |row|
        fo.puts(row.join(delimiter))
      end
    end
  end
  def [](ix)
    @fields[fieldIX(ix)]
  end
  def <<(row)
    raise "ERROR::I expected #{@fields.length} elements in a row, but you gave me #{row.length}" if @fields.length!=row.length
    @fields.zip(row) do |field,el|
      field << el
    end
    nil
  end
  def each(*fns)
    raise "ERROR::no block passed to Dataset#each" if !block_given?
    fns=[(0...@fields.length).to_a] if fns.empty?
    fns2=fns.flatten
    ixs=fields2IXS(fns2)
    if ixs.include?(nil)
      ixs.zip(fns2).each do |ix,fn|
        puts("ERROR::Could not find field \"#{fn}\"") if !ix
      end
      raise "STOP"
    end
    @fields.values_at(*ixs).transpose.each do |res|
      ress=fns.collect do |fn|
        if Array===fn
          res.slice!(0,fn.length)
        else
          res.slice!(0)
        end
      end
      yield(*ress)
    end
  end
  def hash(key,value)
    res={}
    each(key,value) do |k,v|
      res[k]=v
    end
    res
  end
  def sortAsc!(ix)
    six=self[ix].sortIndex
    @fields.each do |field|
      field.sortBy!(six)
    end
    self
  end
  def sortDesc!(ix)
    six=self[ix].sortReverseIndex
    @fields.each do |field|
      field.sortBy!(six)
    end
    self
  end
  def fieldIX(name)
    case name
    when String
      res=@fieldNames.index(name)
      if !res
        puts "WARNING::could not find the field (#{name}) in the data set."
        puts(@fieldNames.collect{|fn|"(#{fn})"}.join(""))
      end
      return res
    when Fixnum
      return name
    when Bignum
      return name
    else
      raise "ERROR::cannot convert #{name} to an index"
    end
  end
  def fields2IXS(ixs)
    result=nil
    if Array===ixs
      result=ixs.collect do |ix|
        case ix
        when Fixnum
          ix
        when String
          tmp=@fieldNames.index(ix)
          raise("ERROR::could not find field \"#{ix}\" in the data set.") if tmp==nil
          tmp
        when NilClass
          puts("WARNING::I found a \"nil\" in fields2IXS")
        else
          raise "ERROR::You can only specify the fields you want either by index or column name (#{ix.class})."
        end
      end
    else
      case ixs
      when Fixnum
        result=[ixs]
      when String
        result=[@fieldNames.index(ixs)]
        puts("WARNING::could not find field \"#{ixs}\" in the data set.") if result[0]==nil
      when NilClass
        result=[]
      else
        raise "ERROR::You can only specify the fields you want either by index or column name."
      end
    end
    return result
  end
  # Splits the data set into different data sets according to one field
  def split(name)
    newDataSets = Hash.new{|h,k|h[k] = Dataset.new(@fieldNames,@delimiter)}
    ix = fieldIX(name)
    each do |values|
      key = (block_given? ? yield(values[ix]) : values[ix])
      newDataSets[key] << values
    end
    newDataSets
  end
end

if __FILE__==$0
  fnData="testData.tsv"
  begin
    "a\tb\tc\n1\t2\t3\n4\t5\t6".export(fnData)
    data=Dataset.load(fnData)
    puts data
    puts data["b"]
    h=data.hash("a","c")
    puts h
    data.sortDesc!("c")
    puts data
  ensure
    File.delete(fnData)
  end
end
