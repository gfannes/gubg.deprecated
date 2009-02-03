require("tools")

class Rinle
  def initialize
    @nc = nil
    @root, @base, @current = nil, nil, nil
  end
  def run
    NCurses.use do |nc|
      setNCurses(nc)
      loop do
        show
        command = getCommand
        command.execute if !command.nil?
      end
    end
  end

  def getCommand
  end
  def show
    @nc.puts(0, 0, @base.class.to_s)
  end
  def setNCurses(nc)
    @nc = nc
    @nc.show
  end
end

if __FILE__ == $0
  rinle = Rinle.new
  rinle.run
end

