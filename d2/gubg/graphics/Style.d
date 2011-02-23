module gubg.graphics.Style;

import gubg.Format;

struct Color
{
    // Some predefined colors
    static const Color black   = Color(0.0, 0.0, 0.0);
    static const Color white   = Color(1.0, 1.0, 1.0);
    static const Color red     = Color(1.0, 0.0, 0.0);
    static const Color green   = Color(0.0, 1.0, 0.0);
    static const Color blue    = Color(0.0, 0.0, 1.0);
    static const Color purple  = Color(1.0, 0.0, 1.0);
    static const Color cyan    = Color(0.0, 1.0, 1.0);
    static const Color yellow  = Color(1.0, 1.0, 0.0);
    static const Color gray    = Color(0.5, 0.5, 0.5);
    static const Color coolRed  = Color(0.33, 0.07, 0.20);
    static const Color darkRed  = Color(0.25, 0.06, 0.15);
    static const Color coolGreen = Color(0.20, 0.33, 0.07);
    static const Color darkGreen = Color(0.15, 0.25, 0.06);
    static const Color coolBlue  = Color(0.07, 0.20, 0.33);
    static const Color darkBlue  = Color(0.06, 0.15, 0.25);

    const real InvalidValue = -2.0;
    static const Color invalid = Color(InvalidValue, InvalidValue, InvalidValue);

    //Constructors
    this (real rr, real gg, real bb)
	{
	    r = rr;
	    g = gg;
	    b = bb;
	}
    this (real[] rgb)
	{
	    this(rgb[0], rgb[1], rgb[2]);
	}
    this (real shade)
	{
	    if (shade > 0)
            this(0, shade, 0);
	    else
            this(-shade, 0, 0);
	}
    real[] rgb(){return [r, g, b];}

    ubyte redByte(){return cast(ubyte)(255*r);}
    ubyte greenByte(){return cast(ubyte)(255*g);}
    ubyte blueByte(){return cast(ubyte)(255*b);}
    ubyte[] rgbByte(){return [cast(ubyte)(255*r), cast(ubyte)(255*g), cast(ubyte)(255*b)];}

    bool isValid(){return InvalidValue != r;}
    
    string toString() const {return Format.immediate("#(%s, %s, %s)", r, g, b);}

    // Data fields
    real r;
    real g;
    real b;
}

struct Style
{
    ref Style fill(Color fill)
    {
        fill_ = fill;
        return this;
    }
    ref Style stroke(Color stroke)
    {
        stroke_ = stroke;
        return this;
    }
    ref width(real width)
    {
        width_ = width;
        return this;
    }
    bool hasFill(){return fill_.isValid();}
    bool hasStroke(){return stroke_.isValid() && InvalidWidth != width_;}
    Color fillColor(){assert(hasFill()); return fill_;}
    Color strokeColor(){assert(hasStroke()); return stroke_;}
    real strokeWidth(){assert(hasStroke()); return width_;}

    string toString()
    {
        Format format;
        format.delimiter = ", ";
        if (hasFill())
            format("Fill color: %s", fill_);
        if (hasStroke())
            format("Stroke color: %s, stroke width: %s", stroke_, width_);
        return format.toString();
    }

    private:
    Color fill_ = Color.invalid;
    Color stroke_ = Color.invalid;
    const real InvalidWidth = -1;
    real width_ = InvalidWidth;
}

version (UnitTest)
{
    import std.stdio;
    void main()
    {
        writeln(Color.black.toString);
        writeln(Color.red.toString);

        auto s = Style().fill(Color.red);
        writeln(s.toString());
        s.stroke(Color.green);
        writeln(s.toString());
        s.width(3);
        writeln(s.toString());
    }
}
