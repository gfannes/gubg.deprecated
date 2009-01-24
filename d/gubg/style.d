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
	    mStroke = stroke;
	    mStrokeWidth = strokeWid;
	    mStrokeColor = strokeCol;
	    mFill = fill;
	    mFillColor = fillCol;
	}
    static Style defaultStyle()
	{
	    Style style = new Style(true, 1.0, Color.White, false, Color.Invalid);
	    return style;
	}

    // Getters
    bool stroke(){return mStroke;}
    real strokeWidth(){synchronized(this){return mStrokeWidth;}}
    Color strokeColor(){synchronized(this){return mStrokeColor;}}
    bool fill(){synchronized(this){return mFill;}}
    Color fillColor(){synchronized(this){return mFillColor;}}

    // Setters
    void setStroke(bool stroke){synchronized(this){mStroke = stroke;}}
    void setStrokeWidth(real strokeWidth){synchronized(this){mStrokeWidth = strokeWidth;}}
    void setStrokeColor(Color color){synchronized(this){mStrokeColor = color;}}
    void setFill(bool fill){synchronized(this){mFill = fill;}}
    void setFillColor(Color color){synchronized(this){mFillColor = color;}}

    // duplicate
    Style dup()
	{
	    return new Style(mStroke, mStrokeWidth, mStrokeColor, mFill, mFillColor);
	}

    void print()
	{
	    puts("Style:");
	    if (mStroke)
	    {
		putsn("\tStroke color:\t"); mStrokeColor.print();
		puts("\tStroke width:\t{}", mStrokeWidth);
	    }
	    if (mFill)
		putsn("\tFill color:\t"); mFillColor.print();
	}

private:
    bool mStroke;
    real mStrokeWidth;
    Color mStrokeColor;
    bool mFill;
    Color mFillColor;
}
