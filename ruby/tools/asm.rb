class ASMSection
  def initialize(name)
    @name = name
  end
  def writeHeader(fo)
    fo.puts(".section .#{@name}")
  end
  def write(fo)
    writeHeader(fo)
    writeSection(fo)
    fo.puts("")
  end
end

class DataSection < ASMSection
  class AsciiElement
    def initialize(name, init)
      @name, @init = name, init
    end
    def write(fo)
      fo.print("#{@name}:\n\t.ascii")
      if @init.nil?
        fo.puts("")
      else
        initialValue = @init.gsub("\n", "\\n")
        fo.puts("\t\"#{initialValue}\"")
      end
    end
  end
  def initialize
    super("data")
    @elements = []
  end
  def add(type, name, init = nil)
    el = nil
    case type
    when :ascii
      el = AsciiElement.new(name, init)
    else
      raise "ERROR::Unknown data type #{type}"
    end
    @elements << el
  end
  def writeSection(fo)
    @elements.each do |el|
      el.write(fo)
    end
  end
end

class BSSSection < ASMSection
  def initialize
    super("bss")
  end
  def addGlobal(name)
    @global = name
  end
  def writeSection(fo)
    if @global
      fo.puts(".global\t#{@global}")
    end
  end
end

class TextSection < ASMSection
  class Function
    def initialize(name, coms = [])
      @name, @commands = name, []
      coms.each do |command|
        add(command)
      end
    end
    def add(command)
      case command
      when String
        @commands << command
      when Array
        command.each do |com|
          add(com)
        end
      else
        raise "ERROR::Unknown command #{command}"
      end
    end
    def write(fo)
      fo.print("#{@name}:\n")
      @commands.each do |command|
        fo.puts("\t#{command}")
      end
    end
  end
  def initialize
    super("text")
    @functions = []
  end
  def addFunction(name, commands = [])
    func = Function.new(name, commands)
    @functions << func
    func
  end
  def writeSection(fo)
    @functions.each do |function|
      function.write(fo)
    end
  end
end

class ASM
  attr :data, true
  attr :bss, true
  attr :text, true
  def initialize
    @data = DataSection.new
    @bss = BSSSection.new
    @text = TextSection.new
  end

  def export(fileName)
    File.open(fileName, "w") do |fo|
      @data.write(fo)
      @bss.write(fo)
      @text.write(fo)
    end
  end
end

if __FILE__ == $0
  # Create the assembler program
  asm = ASM.new
  asm.data.add(:ascii, "startMessage", "Hello World\n")
  asm.bss.addGlobal("_start")
  start = asm.text.addFunction("_start")
  start.add([
              "movl $12, %edx  # Write start message",
              "movl $startMessage, %ecx",
              "movl $1, %ebx",
              "movl $4, %eax",
              "int $0x80",
              "movl $0, %ebx   # Exit",
              "movl $1, %eax",
              "int $0x80",
            ])

  # Export it
  asm.export("test.s")

  # Compile and link it
  `as test.s -o test.o`
  `ld test.o`

  # Run it
  system("./a.out")
end
