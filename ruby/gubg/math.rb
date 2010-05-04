class Array
  def sum
    inject{|s,el|s+el}
  end
end

def drawExponential(lambda)
	-Math.log(rand)/lambda
end
