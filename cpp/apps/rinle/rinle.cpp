#include "rinle/presenter/Commander.hpp"
#include "rinle/view/Window.hpp"
#include "rinle/model/File.hpp"

int main()
{
    rinle::model::File file(__FILE__);
    rinle::view::Window window;
	rinle::presenter::Commander commander;

	window.addObserver(commander.observer);
	commander.addObserver(file.observer);
	file.addObserver(window.observer);

//comment
    rinle::view::run();
    return 0;
} 
