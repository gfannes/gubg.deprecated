#this is the base for building
$compileOptionsPerInclude={
  'SDL/SDL.h' => "`sdl-config --cflags`",
  'cairomm/context.h' => "`pkg-config --cflags cairomm-1.0`",
  'cairomm/surface.h' => "`pkg-config --cflags cairomm-1.0`",
#   'posteriorLogProb.hpp' => "-pg",
}
if mingw?
  $compileOptionsPerInclude['pthread.h'] = "-Ic:/users/befanneg/software/pthreads.2"
#  $compileOptionsPerInclude['SDL/SDL.h'] = "-IC:/users/befanneg/software/SDL-1.2.13/include -D_REENTRANT -Dmain=SDL_main"
  $compileOptionsPerInclude['SDL/SDL.h'] = "-IC:/users/befanneg/software/SDL-1.2.13/include -D_GNU_SOURCE=1 -Dmain=SDL_main"

  cairoIncludes = "-IC:/users/befanneg/software/GTKmm/include/cairomm-1.0 -IC:/users/befanneg/software/GTK/include -IC:/users/befanneg/software/GTK/include/cairo"
  $compileOptionsPerInclude['cairomm/context.h'] = cairoIncludes
  $compileOptionsPerInclude['cairomm/surface.h'] = cairoIncludes
end

$buildOptionsPerInclude={
  'SDL/SDL.h' => "`sdl-config --libs`",
  'cairomm/context.h' => "`pkg-config --libs cairomm-1.0`",
  'cairomm/surface.h' => "`pkg-config --libs cairomm-1.0`",
  'pthread.h' => "-l pthread",
  'tre/regex.h' => "-LC:/users/befanneg/software/1.0/local/lib -ltre",
#   'canvas.hpp' => "-pg",
}
if mingw?
  $buildOptionsPerInclude['pthread.h'] = "-Lc:/users/befanneg/software/pthreads.2 -lpthread"
#  $buildOptionsPerInclude['SDL/SDL.h'] = "-lmingw32 -LC:/users/befanneg/software/SDL-1.2.13/lib -lSDL -Lc:/users/befanneg/software/pthreads.2 -lpthread -lSDLmain -mwindows"
  $buildOptionsPerInclude['SDL/SDL.h'] = "-LC:/users/befanneg/software/SDL-1.2.13/lib -lmingw32 -lSDLmain -lSDL -mwindows"

  cairoLibs = "-LC:/users/befanneg/software/GTKmm/lib -lcairomm-1.0 -LC:/users/befanneg/software/GTK/lib -lcairo"
  $buildOptionsPerInclude['cairomm/context.h'] = cairoLibs
  $buildOptionsPerInclude['cairomm/surface.h'] = cairoLibs
  $buildOptionsPerInclude['socket.hpp'] = "-lws2_32"
end

$doLineLevels = {
   /optimalization/ => 2,
  /vector/ => 2,
#   /neuron/ => 0,
#  /./ => 10,
}
