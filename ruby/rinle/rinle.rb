require("tools")
require("focus")
require("patterns/chainOfResponsibility")
require("commands")

class Rinle < IChainOfResponsibility
  def initialize
    @nc = nil
    @root, @base, @current = nil, nil, nil
  end
  def run
    NCurses.use do |nc|
      setNCurses(nc)
      @ok = true
      while @ok
        show
        command = getCommand
        command.execute if !command.nil?
      end
    end
  end

  def getCommand
    @focus.handle(:getCommand)
  end
  def show
    @nc.puts(0, 0, @base.class.to_s)
  end
  def setNCurses(nc)
    @nc = nc
    @focus = Focus.new(@nc, self)
    @nc.show
  end
  def handle(request)
    case request
    when :getCommand
      case @nc.getKey
      when "q".ord
        QuitCommand.new(self)
      else
        nil
      end
    else
      raise "ERROR::Unknown request"
    end
  end
  def quit
    @ok = false
  end
end

if __FILE__ == $0
  rinle = Rinle.new
  rinle.run
end

