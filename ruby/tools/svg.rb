# This file implements functionality to create svg images
require("tools/builder.rb")
require("tools/utils.rb")

module SVG
  # The SVG document
  class Document
    # range: [minX, minY, maxX, maxY] coordinates of lower left and upper right corners
    # dimensions: [width, height] dimensions the svg should use for rendering
    def initialize(range, args = {:dimensions => nil})
      dimensions = args[:dimensions]||[100,100]
      @range, @width, @height = range, dimensions[0], dimensions[1]
      @transform = Transform.new(@range, dimensions)
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
      File.open(fileName,"w") do |fo|
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
    # x,y: coordinates of the center
    def initialize(x, y, w, h, style = nil)
      @x,@y,@w,@h,@style = x,y,w,h,style
    end
    def generate(parent, transform)
      x,y = transform.coordinates(@x,@y)
      w,h = transform.sizes(@w,@h)
      parent.rect(createStyle(@style), "x" => x-w*0.5, "y" => y-h*0.5, "width" => w, "height" => h)
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
            Rectangle.new(xStart+0.5*widthH,@y,widthH,@h,style).generate(gg,transform)
            Text.new(@labelProc.call(key),xStart+0.01*@w,@y-0.4*@fontFrac*@h,"font-size" => @fontFrac*@h).generate(gg,transform)
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
    def initialize(range, dimensions)
      @ax = (dimensions[0]-1)/(range[2]-range[0]).to_f
      @bx = -@ax*range[0]
      @ay = (dimensions[1]-1)/(range[1]-range[3]).to_f
      @by = -@ay*range[3]
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
  svg = Document.new([0,0,10,10], :dimensions => [800,600])
  svg << Rectangle.new(5,5,8,8,:fill => 'yellow')
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
  puts svg.generate
  svg.export("test.svg")
end
