require("gubg/utils")

module Gubg
	class Histogram
		attr(:bins, true)
		attr(:counts, true)
		attr(:domain, true)
		attr(:range, true)

		def initialize(values, nrBins)
			@nrBins = nrBins
			computeValuesLocations_(values)
		end

		def each
			@bins.zip(@counts).each do |bin, count|
				yield(bin, count)
			end
		end
		def to_s
			res = ""
			each do |bin, count|
				res += "#{bin}\t#{count}\n"
			end
			res
		end

		def computeValuesLocations_(values)
			mi, ma = values.minmax
			@domain = (mi .. ma)
			width = (ma - mi).to_f
			@binWidth = width / @nrBins
			@bins = (0...@nrBins).collect do |i|
				if i == @nrBins-1
					((mi + i*@binWidth) .. (mi + (i+1)*@binWidth) )
				else
					((mi + i*@binWidth) ... (mi + (i+1)*@binWidth) )
				end
			end
			@counts = Array.new(@nrBins, 0)
			values.each do |v|
				binIX = (((v - mi)/(width))*@nrBins).floor
				binIX = @nrBins-1 if binIX >= @nrBins
				@counts[binIX] += 1
			end
			@range = (0 .. @counts.max)
		end
	end
end

if __FILE__ == $0
	histogram = Gubg::Histogram.new([1, 1, 1, 2, 3, 3], 3)
	puts("#{histogram}")
end
