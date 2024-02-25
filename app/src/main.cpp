#include <iostream>
#include <flecs.h>
#include <direct.h>
#include <filesystem>
#include <SFML/Graphics.hpp>
#include "factories.h"
#include "grid.h"

using namespace std;


int main(int, char* []) {
	sf::RenderWindow window(sf::VideoMode(800, 600), "My window");

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		window.clear(sf::Color::Black);

		sf::CircleShape circle(50.0f);
		circle.setFillColor(sf::Color(100, 250, 50));

		window.draw(circle);
		window.display();
	}
}