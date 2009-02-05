class IChainOfResponsibility
  attr :successor, true
  def handle(request)
    raise "ERROR::#{self.class}#handle cannot handle the request #{request.class} and has no successor set." if @successor.nil?
    @successor.handle(request)
  end
end
