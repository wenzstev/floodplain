#include <iostream>
#include <flecs.h>
#include <direct.h>
#include <filesystem>
#include <SFML/Graphics.hpp>
#include "factories.h"
#include "grid.h"
#include "flecs_game.h"
#include "flecs_components_transform.h"
#include "canvas.h"


using namespace std;

struct Point {
	float x;
	float y;
};

struct Mag
{
	float m;
};



struct Window
{
	sf::RenderWindow* window;
};

canvas2d::canvas2d(flecs::world& world)
{
	world.component<Circle>()
		.member<float>("X Coordinate")
		.member<float>("Y Coordinate")
		.member<float>("Radius");

	world.component<ScreenDims>()
		.on_set(init_window)
		.member<int>("width")
		.member<int>("height")
		.member<string>("name");

	world.component<Screen>();

}

void canvas2d::init_window(flecs::entity screen, ScreenDims& screenConfig)
{
	sf::RenderWindow window(sf::VideoMode(screenConfig.xPixels, screenConfig.yPixels), screenConfig.screenName);
	flecs::world world = screen.world();
	world.set<Screen>({ &window });
}

int main(int, char* []) {

	flecs::world world;
	sf::RenderWindow window(sf::VideoMode(800, 600), "My window");

	world.import<canvas2d>();

	world.set<Window>({&window});

	auto c = world.entity("Circle")
		.set<Point>({ 5, 5 })
		.set<Mag>({ 40 });

	flecs::system draw = world.system<canvas2d::Circle, canvas2d::Screen>()
		.term_at(2).singleton()
		.each([](flecs::entity e, Point& p, Mag& m, Window& w)
			{
				sf::CircleShape shape(m.m);
				shape.setPosition(p.x, p.y);
				w.window->draw(shape);
			});

	flecs::system clearScreen = world.system<canvas2d::Screen>()
		.kind(flecs::PreUpdate)
		.each([](flecs::entity e, canvas2d::Screen& s)
			{
				s.canvas->clear(sf::Color::Black);

			});

	flecs::system drawScreen = world.system<canvas2d::Screen>()
		.kind(flecs::PostUpdate)
		.each([](flecs::entity e, canvas2d::Screen& s)
			{
				s.canvas->display();
			});

	auto next = world.entity("SecondCircle")
		.set<Point>({ 50, 399 })
		.set<Mag>({ 100 });

	auto third = world.entity()
		.set<Point>({ 500, 40 })
		.set<Mag>({ 60 });


	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear(sf::Color::Black);

		world.progress();

		window.display();
	}

	return world.app().enable_rest().run();

}