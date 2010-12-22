class Array
  def sum
    inject{|s,el|s+el}
  end
end

def drawExponential(lambda)
	-Math.log(rand)/lambda
end

def drawBernoulli(probSuccess)
	rand <= probSuccess
end

def drawCategorical(probabilities, categories = nil)
	prob = rand
	cumulProb = 0
	if categories
		probabilities.zip(categories).each do |p, category|
			cumulProb += p
			return category if prob <= cumulProb
		end
	else
		probabilities.each_with_index do |p, ix|
			cumulProb += p
			return ix if prob <= cumulProb
		end
	end
	raise("Your probabilities do not add to 1 (#{probabilities}, #{prob})")
end
