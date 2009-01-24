class Walker
  def initialize(mod)
    extend(mod)
  end

  def walk(obj)
    prepareWalking
    walk4Walker(obj)
    finishWalking
  end
end
