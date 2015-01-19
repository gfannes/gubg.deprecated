#include "SFML/Graphics.hpp"
#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

namespace
{
	template <typename Pos>
	void print(const char *cap, const Pos &pos)
	{
		cout << cap << " " << pos.x << " " << pos.y << endl;
	}
}

int main()
{
	sf::RenderWindow rw(sf::VideoMode(640, 480), "Mouse getPosition test");

	while (rw.isOpen())
	{
		//Process events
		{
			sf::Event evt;
			while (rw.pollEvent(evt))
			{
				if (evt.type == sf::Event::Closed)
					return 0;
			}
		}

		//Draw
		{
			rw.clear();
			rw.display();
		}

		//These operations should not move the window
		rw.setPosition(rw.getPosition());

		//print("mouse ", sf::Mouse::getPosition());
		//print("window", rw.getPosition());

		this_thread::sleep_for(chrono::milliseconds(500));

	}

	return 0;
}