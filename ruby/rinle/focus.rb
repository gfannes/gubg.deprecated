class Focus
  def initialize(defaultHandler)
    defaultHandler.successor = nil
    @handlers = [defaultHandler]
  end
  def set(handler)
    handler.successor = @handlers.last
    @handlers.push(handler)
    yield
    @handlers.pop
    handler.successor = nil
  end
  def handle(request)
    @handlers.last.handle(request)
  end
end
