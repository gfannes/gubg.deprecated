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
enum cairo_font_slant_t
{
    CAIRO_FONT_SLANT_NORMAL,
    CAIRO_FONT_SLANT_ITALIC,
    CAIRO_FONT_SLANT_OBLIQUE
}
enum cairo_font_weight_t
{
    CAIRO_FONT_WEIGHT_NORMAL,
    CAIRO_FONT_WEIGHT_BOLD
}
struct cairo_font_extents_t
{
    double ascent;
    double descent;
    double height;
    double max_x_advance;
    double max_y_advance;
}
struct cairo_font_face_t {}

extern (C)
{
    //cairo_surface_t functions
    cairo_surface_t *cairo_image_surface_create_for_data (ubyte *data,
            cairo_format_t format,
            int width,
            int height,
            int stride);
    void cairo_surface_destroy(cairo_surface_t *surface);

    //cairo_t funtions
    cairo_t * cairo_create (cairo_surface_t *target);
    void cairo_destroy(cairo_t *cr);

    //Drawing functions
    void cairo_move_to(cairo_t *cr, double x, double y);
    void cairo_line_to(cairo_t *cr, double x, double y);
    void cairo_stroke(cairo_t *cr);
    void cairo_arc (cairo_t *cr, double xc, double yc, double radius, double angle1, double angle2);
    void cairo_fill(cairo_t *cr);
    void cairo_set_line_width(cairo_t *cr, double width);
    void cairo_set_source_rgb(cairo_t *cr, double red, double green, double blue);

    //Font functions
    void cairo_select_font_face(cairo_t *cr, const char *family, cairo_font_slant_t slant, cairo_font_weight_t weight);
    void cairo_set_font_size(cairo_t *cr, double size);
    void cairo_show_text(cairo_t *cr, const char *utf8);
    void cairo_font_extents(cairo_t *cr, cairo_font_extents_t *extents);
    cairo_font_face_t *cairo_get_font_face(cairo_t *cr);
    char *cairo_toy_font_face_get_family(cairo_font_face_t *font_face);
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
