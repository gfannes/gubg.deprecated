require("ncurses")

class NCurses
  attr(:nrRows, true)
  attr(:nrCols, true)

  # Start ncurses just for the scope of the passed block
  def NCurses.use(bgColor = :black)
    begin
      nc = NCurses.new(bgColor)
      yield(nc)
    ensure
      nc.exit
    end
  end

  def initialize(bgColor = :black)
    # Create the colors
    @@colors = {
      black: Ncurses::COLOR_BLACK,
      blue: Ncurses::COLOR_BLUE,
      cyan: Ncurses::COLOR_CYAN
    }
    @bgColor = bgColor

    # Start ncurses
    Ncurses.initscr
    Ncurses.noecho
    Ncurses.cbreak
    if (Ncurses.has_colors?)
      @bgColor = @@colors[@bgColor]
      Ncurses.start_color
      if (Ncurses.respond_to?("use_default_colors"))
        if (Ncurses.use_default_colors == Ncurses::OK)
          @bgColor = -1
        end
      end
      createColor(1, :blue)
      createColor(1, :cyan)
    end
    @screen = Ncurses.stdscr
    @nrCols = @screen.getmaxx
    @nrRows = @screen.getmaxy
    @window = Ncurses::WINDOW.new(@nrRows, @nrCols, 0, 0)
    
    # Put something on the screen
    clear
    @nrRows.times do |r|
      puts(r, 0, '?'*@nrCols)
    end
    show
  end

  def exit
    if @screen
      Ncurses.echo
      Ncurses.nocbreak
      Ncurses.nl
      Ncurses.endwin
      @screen=nil
    else
      puts("Nothing to exit for ncurses")
    end
  end

  # Functions that have todo with output
  def clear
    @window.clear
  end
  def puts(r, c, str, args = {})
    @window.mvaddstr(r, c, str)
    show if !args[:noShow]
  end
  def show
    @window.refresh
  end
  def createColor(color, fg, bg = nil)
    fg = @@colors[fg] if Symbol === fg
    bg = @bgColor if bg.nil?
    bg = @@colors[bg] if Symbol === bg
    Ncurses::init_pair(color, fg, bg)
  end
  

  # Functions that have todo with input
  # Get a single character
  def getKey
    Ncurses.noecho
    Ncurses.cbreak
    @window.getch
  end
  
end

if __FILE__ == $0
  NCurses.use do |nc|
    while (k = nc.getKey) != "q".ord
      nc.nrRows.times do |r|
        nc.nrCols.times do |c|
          nc.puts(r, c, k.chr)
        end
      end
    end
  end
end
