# The command pattern

class ICommand
  def execute
    raise "ERROR::#{self.class}#execute is not implemented"
  end
end

if __FILE__ == $0
  class Test < ICommand
    def execute
    end
  end
  command = Test.new
  command.execute
end
