# This file implements functionality to create svg images
#  * Supported shapes: Rectangle, Path, Line, Ellipse, Circle, StackBar, Histogram, Text, Link
require("gubg/builder")
require("gubg/utils")
require("gubg/histogram")

module SVG
	# Create a color string
	def color(r, g, b)
		"\#%0.2x%0.2x%0.2x"%[r*255, g*255, b*255]
	end
	# The SVG document
	class Document
		# rangeX, rangeY: Range of the X and Y axis to show
		# dimensions: [width, height] dimensions the svg should use for rendering
		def initialize(rangeX, rangeY, args = {:dimensions => nil})
			dimensions = args[:dimensions]||[100,100]
			@width, @height = dimensions[0], dimensions[1]
			@transform = Transform.new(rangeX, rangeY, dimensions)
			@shapes = []
		end
		def <<(shape)
			@shapes << shape
		end
		def generate(target = nil)
			target ||= ""
			xml = Builder::XmlMarkup.new(:target => target,:indent=>2)
			xml.instruct!("xml version=\"1.0\" standalone=\"no\"")
			xml << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">"
			xml.svg("width" => @width, "height" => @height, "version" => "1.1", "xmlns" => "http://www.w3.org/2000/svg", "xmlns:xlink" => "http://www.w3.org/1999/xlink") do |svg|
				svg.g do |g|
					@shapes.each do |shape|
						shape.generate(g, @transform)
					end
				end
			end
			return target
		end
		def export(fileName)
			File.open(fileName, "w") do |fo|
				generate(fo)
			end
		end
	end

	# SVG shapes derive from this class
	class Shape
		# transform is can be used to transform coordinates and sizes to the SVG coordinate system
		def generate(parent, transform)
			raise "ERROR::Please implement your own generate function for #{self.class}"
		end
		# Each shape can add its own style. Use the class as key
		@@defaultStyles = {
			nil => {
			"fill" => "none",
			"stroke" => "black",
			"stroke-width" => 1,
		}
		}
		# Each shape can implement specific transforms on the style. Use the class as key
		@@styleTransforms = {}

		def setDefaultStyle(klass, style)
			@@defaultStyles[klass] = style
		end
		def createStyle(style, transform = nil)
			defaultStyle = @@defaultStyles[self.class]||@@defaultStyles[nil]
			defaultStyle = if style
					       # Replace all symbol keys with strings
					       cleanedStyle = {}
					       style.each do |k,v|
						       cleanedStyle[k.to_s] = v
					       end
					       defaultStyle.merge(cleanedStyle)
				       else
					       defaultStyle.dup
				       end
			# Allow for last-minute modifications to the style, based on the transform
			if styleTransform = @@styleTransforms[self.class]
				styleTransform.call(defaultStyle,transform)
			end
			defaultStyle
		end
	end

	# A rectangle shape
	class Rectangle < Shape
		# x, y: coordinates of left bottom
		def initialize(x, y, w, h, style = nil)
			@x, @y, @w, @h, @style = x+0.5*w, y+0.5*h, w, h, style
		end
		# x, y: coordinates of the center
		def Rectangle.centered(x, y, w, h, style = nil)
			Rectangle.new(x-0.5, y-0.5, w, h, style)
		end
		def generate(parent, transform)
			x, y = transform.coordinates(@x, @y)
			w, h = transform.sizes(@w, @h)
			parent.rect(createStyle(@style), "x" => x-w*0.5, "y" => y-h*0.5, "width" => w, "height" => h)
		end
	end

	# A path
	class Path < Shape
		def initialize(xs, ys, close, style = nil)
			@xs, @ys, @close, @style = xs, ys, close, style
		end
		def Path.createFrom(obj, style = nil)
			case obj
			when Gubg::Histogram
				histogram = obj
				xs, ys = [histogram.domain.first], [histogram.range.first]
				histogram.each do |bin, count|
					xs << bin.first << bin.end
					ys << count << count
				end
				xs << histogram.domain.end
				ys << histogram.range.first
				Path.new(xs, ys, false, style)
			else
				raise("Don't know how to create a path from a #{obj.class}")
			end
		end
		def Path.createHistogram(values, nrBins, style = nil)
			Path.createFrom(Gubg::Histogram.new(values, nrBins), style)
		end

		def generate(parent, transform)
			d = []
			@xs.zip(@ys).each_with_index do |xy, ix|
				xx, yy = transform.coordinates(*xy)
				if ix == 0
					d << "M#{xx},#{yy}"
				else
					d << "L#{xx},#{yy}"
				end
			end
			d << "Z" if @close
			parent.path(createStyle(@style), "d" => d.join(' '))
		end
	end

	# A line
	class Line < Shape
		def initialize(co1, co2, style = nil)
			@co1, @co2, @style = co1, co2, style
		end
		def generate(parent, transform)
			tco1, tco2 = transform.coordinates(*@co1), transform.coordinates(*@co2)
			parent.line(createStyle(@style), "x1" => tco1[0], "y1" => tco1[1], "x2" => tco2[0], "y2" => tco2[1])
		end
	end

	# An ellipse
	class Ellipse < Shape
		def initialize(center, radii, style = nil)
			@center, @radii, @style = center, radii, style
		end
		def generate(parent, transform)
			tc = transform.coordinates(*@center)
			tr = transform.sizes(*@radii)
			parent.ellipse(createStyle(@style), "cx" => tc[0], "cy" => tc[1], "rx" => tr[0], "ry" => tr[1])
		end
	end
	# A Circle
	class Circle < Ellipse
		def initialize(center, radius, style = nil)
			super(center, [radius, radius], style)
		end
	end

	# A stacked bar shape
	class StackBar < Shape
		# :fontFrac is the font size to be used as a fraction of the height
		# :labelProc can be a Proc to be used to generate the label that will be shown
		def initialize(x,y,w,h,values,args = {:order => nil, :colors => nil, :style => nil, :fontFrac => nil, :labelProc => nil})
			@x,@y,@w,@h = x,y,w,h
			@style = args[:style]
			@colors = args[:colors]
			@fontFrac = args[:fontFrac]||0.3
			@labelProc = args[:labelProc]||Proc.new{|label|label.to_s}
			@fractions = values.fractions
			order = args[:order]||values.keys.sort
			@keys = order.select{|k|@fractions[k]>0}
		end
		def generate(parent, transform)
			xMin = @x-0.5*@w
			parent.g do |g|
				xStart = xMin
				@keys.each do |key|
					frac = @fractions[key]
					widthH = @w*frac
					if @colors
						if @colors[key]
							style = (@style||{}).merge({:fill => @colors[key]})
						else
							puts("WARNING::No color given for \"#{key}\"")
						end
					end
					g.g do |gg|
						Rectangle.centered(xStart+0.5*widthH,@y,widthH,@h,style).generate(gg, transform)
						Text.new(@labelProc.call(key),xStart+0.01*@w,@y-0.4*@fontFrac*@h,"font-size" => @fontFrac*@h).generate(gg, transform)
					end
					xStart += widthH
				end
			end
		end
	end

	# A text shape
	class Text < Shape
		@@defaultStyles[Text] = {
			"font-family" => "Bitstream Vera Sans",
			"font-size" => 5,
			"fill" => "black",
		}
		@@styleTransforms[Text] = Proc.new do |style, transform|
			xS,yS = transform.sizes(style["font-size"], style["font-size"])
			style["font-size"] = 0.5*(xS+yS)      
		end

		def initialize(text,x,w,style=nil)
			@text,@x,@y,@style = text,x,w,style
		end
		def generate(parent, transform)
			x,y = transform.coordinates(@x,@y)
			parent.text(createStyle(@style, transform), "x" => x, "y" => y) do |text|
				text << @text
			end
		end
	end

	# A hyperlink object
	class Link
		# Set target to "" to open over the current page
		def initialize(url, node, target = "_blank")
			@url, @node, @target = url, node, target
		end
		def generate(parent, transform)
			parent.a("xlink:href" => @url, "target" => @target) do |a|
				@node.generate(a, transform)
			end
		end
	end

	# The transform object to transform coordinates and sizes to SVG coordinates
	class Transform
		def initialize(rangeX, rangeY, dimensions)
			@ax = (dimensions[0]-1)/(rangeX.end - rangeX.first).to_f
			@bx = -@ax*rangeX.first
			@ay = (dimensions[1]-1)/(rangeY.first - rangeY.end).to_f
			@by = -@ay*rangeY.end
		end
		def to_s
			"ax = #{@ax}, bx = #{@bx}, ay = #{@ay}, by = #{@by}"
		end
		def coordinates(x,y=nil)
			if y
				return @ax*x+@bx, @ay*y+@by
			else
				return @ax*x+@bx
			end
		end
		def sizes(width,height=nil)
			if height
				return @ax*width, -@ay*height
			else
				return @ax*width
			end
		end
	end
end

if __FILE__ == $0
	include SVG
	svg = Document.new((0..10), (0..10), :dimensions => [800,600])
	svg << Rectangle.centered(5,5,8,8,:fill => 'yellow')
	(1..8).each do |y|
		svg << Text.new("Some text",1,y,"font-size" => 1)
	end
	svg << StackBar.new(5,5,7,0.3,
			    {:x => 3, :y => 2, :z => 1},
			    :order => [:x,:y,:z], 
			    :colors => {:x => "red", :y => "green", :z => "blue"}, 
			    :style => {:fill => "red"},
			    :fontFrac => 0.5,
			    :labelProc => Proc.new{|label|label.to_s*10})
	svg << Path.new([0, 1, 2, 3, 4], [0, 2, 1, 4, 3], false)
	svg << Line.new([0, 0], [10, 10])
	svg << Circle.new([5, 5], 3)
	svg << Path.createHistogram([1, 1, 1, 2, 3, 3], 3)
	puts svg.generate
	svg.export("test.svg")
end
