#include "rinle/model/File.hpp"
#include "rinle/view/Window.hpp"

int main()
{
    rinle::view::Window window;
    rinle::model::File file("__FILE__");
    auto lines = file.lines();
    window.showLines(lines.begin(), lines.end());
    rinle::view::run();
    return 0;
} 
