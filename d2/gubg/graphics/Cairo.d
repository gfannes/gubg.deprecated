module gubg.graphics.Cairo;

import gubg.bindings.cairo;

class Context
{
    this (ubyte *data, uint width, uint height)
    {
        _targetSurface = new Surface(data, width, height);
        _context = cairo_create(_targetSurface.surface);
    }
    ~this ()
    {
        if (_context !is null)
            cairo_destroy(_context);
        delete _targetSurface;
    }

    void moveTo(real x, real y)
    {
        cairo_move_to(_context, x, y);
    }
    void lineTo(real x, real y)
    {
        cairo_line_to(_context, x, y);
    }
    void stroke()
    {
        cairo_stroke(_context);
    }
    void arc(real xc, real yc, real radius, real angle1, real angle2)
    {
        cairo_arc(_context, xc, yc, radius, angle1, angle2);
    }
    void fill()
    {
        cairo_fill(_context);
    }
    void setLineWidth(real width)
    {
        cairo_set_line_width(_context, width);
    }
    void setSourceRGB(real red, real green, real blue)
    {
        cairo_set_source_rgb(_context, red, green, blue);
    }

    private:
    Surface _targetSurface;
    cairo_t *_context;
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
