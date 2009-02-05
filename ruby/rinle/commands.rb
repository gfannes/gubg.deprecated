require("patterns/command")

class QuitCommand < ICommand
  def initialize(context)
    context.quit
  end
  def execute
  end
end
