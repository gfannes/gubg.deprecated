class Focus
  def initialize(input, defaultHandler)
    @inputCache = InputCache.new(input)
    defaultHandler.successor = nil
    @handlers = [defaultHandler]
  end
  def push(handler)
    handler.successor = @handlers.last
    @handlers << handler
  end
  def pop
    handler = @handlers.pop
    handler.successor = nil
  end
  def handle(request)
    @inputcache.reset(:soft)
    # Before we ask the last handler to handle the request, we insert the @inputCache between the last a next-to-last
    @inputCache.installBetween(@handlers[-1], @handlers[-2])
    @handlers.last.handle(request)
  end
end

class InputCache
  attr :successor, true
  def initalize(input)
    @input = input
    reset(:hard)
  end
  def handle(request)
    reset(:soft)
    tmpSuccessor = @successor
    installBetween(@successor, @successor.successor)
    tmpSuccessor.handle(request)
  end
  def getKey
    key = nil
    @acquiredKeys << @input.getKey if @keyIX < @acquiredKeys.length
    key = @acquiredKeys[@keyIX]
    @keyIX += 1
    key
  end
  def reset(type)
    case type
    when :soft
      @keyIX = 0
    when :hard
      @acquiredKeys = []
      @keyIX = 0
    end
  end
  def installBetween(first, successor)
    
  end
end
