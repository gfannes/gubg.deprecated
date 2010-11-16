module gubg.graphics.Cairo;

import gubg.bindings.cairo;
import std.string;
import std.conv;
import std.stdio;

class Context
{
    this (ubyte *data, uint width, uint height)
    {
        targetSurface_ = new Surface(data, width, height);
        context_ = cairo_create(targetSurface_.surface);
    }
    ~this ()
    {
        if (context_ !is null)
            cairo_destroy(context_);
        delete targetSurface_;
    }

    void moveTo(real x, real y)
    {
        cairo_move_to(context_, x, y);
    }
    void lineTo(real x, real y)
    {
        cairo_line_to(context_, x, y);
    }
    void stroke()
    {
        cairo_stroke(context_);
    }
    void arc(real xc, real yc, real radius, real angle1, real angle2)
    {
        cairo_arc(context_, xc, yc, radius, angle1, angle2);
    }
    void fill()
    {
        cairo_fill(context_);
    }
    void setLineWidth(real width)
    {
        cairo_set_line_width(context_, width);
    }
    void setSourceRGB(real red, real green, real blue)
    {
        cairo_set_source_rgb(context_, red, green, blue);
    }
    void selectFontFace(string family)
    {
        cairo_select_font_face(context_, toStringz(family), cairo_font_slant_t.CAIRO_FONT_SLANT_NORMAL, cairo_font_weight_t.CAIRO_FONT_WEIGHT_NORMAL);
    }
    void setFontSize(real size)
    {
        cairo_set_font_size(context_, size);
    }
    void showText(string text)
    {
        cairo_show_text(context_, toStringz(text));
    }
    void fontExtents(out real ascent, out real descent, out real height)
    {
        cairo_font_extents_t extents;
        writefln("%f %f %f", extents.ascent, extents.descent, extents.height);
        cairo_font_extents(context_, &extents);
        writefln("%f %f %f", extents.ascent, extents.descent, extents.height);
        ascent = extents.ascent;
        descent = extents.descent;
        height = extents.height;
    }
    string toyFontFaceGetFamily()
    {
        cairo_font_face_t *ff = cairo_get_font_face(context_);
        return to!(string)(cairo_toy_font_face_get_family(ff));
    }

    private:
    Surface targetSurface_;
    cairo_t *context_;
}

class Surface
{
    this (ubyte *data, uint width, uint height)
    {
        _surface = cairo_image_surface_create_for_data(data, cairo_format_t.CAIRO_FORMAT_ARGB32, width, height, 4*width);
    }
    ~this ()
    {
        if (_surface !is null)
            cairo_surface_destroy(_surface);
    }

    cairo_surface_t *surface(){return _surface;}

    private:
    cairo_surface_t *_surface;
}

version (UnitTest)
{
    void main()
    {
    }
}
