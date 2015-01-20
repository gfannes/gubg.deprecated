extern crate conrod;
extern crate shader_version;
extern crate event;
extern crate sdl2_window;
extern crate opengl_graphics;
extern crate quack;

use event::{WindowSettings, Events, Event, Ups, MaxFps};
use sdl2_window::{Sdl2Window};
use std::cell::RefCell;
use opengl_graphics::Gl;
use opengl_graphics::glyph_cache::GlyphCache;
use conrod::{Background, Button, Callable, Color, Colorable, Drawable, DropDownList,
	EnvelopeEditor, Frameable, Label, Labelable, NumberDialer, Point, Positionable, Slider,
	Shapeable, TextBox, Theme, Toggle, UiContext, WidgetMatrix, XYPad, };
use quack::Set;

fn main()
{
	println!("Welcome to dolegate");

	let opengl = shader_version::OpenGL::_3_2;
	let window = Sdl2Window::new(
		opengl,
		WindowSettings {
			title: "Dolegate".to_string(),
			size: [1180, 580],
			fullscreen: false,
			exit_on_esc: true,
			samples: 4,
		}
		);
	let window_ref = RefCell::new(window);
	//Uses quack::Set
	let mut event_iter = Events::new(&window_ref).set(Ups(180)).set(MaxFps(60));
	let mut gl = Gl::new(opengl);
	let font_path = Path::new("./assets/Anonymous_Pro.ttf");
	let theme = Theme::default();
	let glyph_cache = GlyphCache::new(&font_path).unwrap();
	let mut uic = UiContext::new(glyph_cache, theme);

	//Event loop
	for event in event_iter {
		uic.handle_event(&event);
		if let Event::Render(args) = event {
			gl.draw([0, 0, args.width as i32, args.height as i32], |_, gl| {
				draw_ui(gl, &mut uic);
			});
		}
	}
}

fn draw_ui(gl: &mut Gl, uic: &mut UiContext)
{
	uic.background().color(Color::black()).draw(gl);	
}