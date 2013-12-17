#include "rinle/model/Model.hpp"
#include "rinle/view/Window.hpp"
#include "rinle/view/Util.hpp"
#include "rinle/presenter/Presenter.hpp"

int main()
{
    rinle::model::Model model;
    rinle::view::Window window(model);
	rinle::presenter::Presenter presenter(model, window);
    presenter.setCurrent(rinle::File(__FILE__));

    rinle::view::run();
    return 0;
} 
