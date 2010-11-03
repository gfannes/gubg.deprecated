module gubg.bindings.cairo;

struct cairo_surface_t {}
struct cairo_t {}

enum cairo_format_t
{
    CAIRO_FORMAT_ARGB32,
    CAIRO_FORMAT_RGB24,
    CAIRO_FORMAT_A8,
    CAIRO_FORMAT_A1
        /* The value of 4 is reserved by a deprecated enum value.
         * The next format added must have an explicit value of 5.
         CAIRO_FORMAT_RGB16_565 = 4,
         */
}

extern (C)
{
    // cairo_surface_t functions
    cairo_surface_t *cairo_image_surface_create_for_data (ubyte *data,
            cairo_format_t format,
            int width,
            int height,
            int stride);
    void cairo_surface_destroy(cairo_surface_t *surface);

    // cairo_t funtions
    cairo_t * cairo_create (cairo_surface_t *target);
    void cairo_destroy(cairo_t *cr);

    // Drawing functions
    void cairo_move_to(cairo_t *cr, double x, double y);
    void cairo_line_to(cairo_t *cr, double x, double y);
    void cairo_stroke(cairo_t *cr);
    void cairo_arc (cairo_t *cr, double xc, double yc, double radius, double angle1, double angle2);
    void cairo_fill(cairo_t *cr);
    void cairo_set_line_width(cairo_t *cr, double width);
    void cairo_set_source_rgb(cairo_t *cr, double red, double green, double blue);
}

version (UnitTest)
{
    void main()
    {
        const uint width = 100;
        const uint height = 100;
        ubyte[4*width*height] data;
        cairo_surface_t *surface = cairo_image_surface_create_for_data(data.ptr, cairo_format_t.CAIRO_FORMAT_ARGB32, width, height, 0);
        cairo_t *cr = cairo_create(surface);
    }
}
