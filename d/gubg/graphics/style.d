module gubg.graphics.style;

import gubg.puts;

import tango.text.convert.Format;

struct Color
{
    // Some predefined colors
    static Color Black  = opCall(0.0, 0.0, 0.0);
    static Color White  = opCall(1.0, 1.0, 1.0);
    static Color Red    = opCall(1.0, 0.0, 0.0);
    static Color Green  = opCall(0.0, 1.0, 0.0);
    static Color Blue   = opCall(0.0, 0.0, 1.0);
    static Color Purple = opCall(1.0, 0.0, 1.0);
    static Color Cool   = opCall(0.07, 0.20, 0.33);

    static Color Invalid = opCall(-2.0, -2.0, -2.0);

    static Color opCall(real r, real g, real b)
	{
	    Color color;
	    color.red = r;
	    color.green = g;
	    color.blue = b;
	    return color;
	}
    static Color opCall(real[] rgb)
	{
	    return opCall(rgb[0], rgb[1], rgb[2]);
	}
    static Color opCall(real shade)
	{
	    if (shade > 0)
		return opCall(0, shade, 0);
	    else
		return opCall(-shade, 0, 0);
	}
    real[] rgb(){return [red, green, blue];}

    ubyte redByte(){return cast(ubyte)(255*red);}
    ubyte greenByte(){return cast(ubyte)(255*green);}
    ubyte blueByte(){return cast(ubyte)(255*blue);}
    ubyte[] rgbByte(){return [cast(ubyte)(255*red), cast(ubyte)(255*green), cast(ubyte)(255*blue)];}
    
    void print(){puts("#({}, {}, {})", red, green, blue);}
    char[] toString(){return Format("#({}, {}, {})", red, green, blue);}


    // Data fields
    real red;
    real green;
    real blue;
}

class StrokeStyle
{
    this(real width, Color color)
	{
	    strokeWidth = width;
	    strokeColor = color;
	}

    StrokeStyle dup(){return new StrokeStyle(_strokeWidth, _strokeColor);}

    // Getters and setters
    real strokeWidth(){synchronized(this){return _strokeWidth;}}
    void strokeWidth(real width){synchronized(this){_strokeWidth = width;}}
    Color strokeColor(){synchronized(this){return _strokeColor;}}
    void strokeColor(Color color){synchronized(this){_strokeColor = color;}}

    static StrokeStyle standard(){return new StrokeStyle(1.0, Color.Black);}

    char[] toString(){return Format("strokeWidth = {}, strokeColor = {}", _strokeWidth, _strokeColor);}

private:
    real _strokeWidth;
    Color _strokeColor;
}

class FillStyle
{
    this(Color fillColor)
	{
	    _fillColor = fillColor;
	}

    FillStyle dup(){return new FillStyle(_fillColor);}

    // Getters and setters
    Color fillColor(){synchronized(this){return _fillColor;}}
    void fillColor(Color color){synchronized(this){_fillColor = color;}}

    static FillStyle standard(){return new FillStyle(Color.Red);}

    char[] toString(){return Format("fillColor = {}", _fillColor);}

private:
    Color _fillColor;
}
