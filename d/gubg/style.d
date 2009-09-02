module gubg.style;

import gubg.puts;

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


    // Data fields
    real red;
    real green;
    real blue;
}

class Style
{
    this(bool stroke, real strokeWid, Color strokeCol, bool fill = false, Color fillCol = Color.Invalid)
	{
	    _stroke = stroke;
	    _strokeWidth = strokeWid;
	    _strokeColor = strokeCol;
	    _fill = fill;
	    _fillColor = fillCol;
	}
    static Style defaultStyle()
	{
	    Style style = new Style(true, 1.0, Color.White, false, Color.Invalid);
	    return style;
	}

    // Getters
    bool stroke(){return _stroke;}
    real strokeWidth(){synchronized(this){return _strokeWidth;}}
    Color strokeColor(){synchronized(this){return _strokeColor;}}
    bool fill(){synchronized(this){return _fill;}}
    Color fillColor(){synchronized(this){return _fillColor;}}

    // Setters
    void setStroke(bool stroke){synchronized(this){_stroke = stroke;}}
    void setStrokeWidth(real strokeWidth){synchronized(this){_strokeWidth = strokeWidth;}}
    void setStrokeColor(Color color){synchronized(this){_strokeColor = color;}}
    void setFill(bool fill){synchronized(this){_fill = fill;}}
    void setFillColor(Color color){synchronized(this){_fillColor = color;}}

    // duplicate
    Style dup()
	{
	    return new Style(_stroke, _strokeWidth, _strokeColor, _fill, _fillColor);
	}

    void print()
	{
	    puts("Style:");
	    if (_stroke)
	    {
		putsn("\tStroke color:\t"); _strokeColor.print();
		puts("\tStroke width:\t{}", _strokeWidth);
	    }
	    if (_fill)
		putsn("\tFill color:\t"); _fillColor.print();
	}

private:
    bool _stroke;
    real _strokeWidth;
    Color _strokeColor;
    bool _fill;
    Color _fillColor;
}
