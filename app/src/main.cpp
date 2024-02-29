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
	sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode(screenConfig.xPixels, screenConfig.yPixels), screenConfig.screenName);
	flecs::world world = screen.world();
	world.set<Screen>({ window });
}

int main(int, char* []) {

	flecs::world world;

	world.import<canvas2d>();

	struct SFMLEvent {
		sf::Event* event;
	};

	flecs::entity screenDims = world.entity()
		.set<canvas2d::ScreenDims>({ 800, 600, "Window in flecs" });

	flecs::system draw = world.system<canvas2d::Circle, canvas2d::Screen>()
		.term_at(2).singleton()
		.each([](flecs::entity e, canvas2d::Circle& c, canvas2d::Screen& s)
			{
				sf::CircleShape shape(c.r);
				shape.setPosition(c.x, c.y);
				s.canvas->draw(shape);
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

	flecs::system pollEvent = world.system<canvas2d::Screen>()
		.each([](flecs::entity e, canvas2d::Screen& s) {
			sf::Event event;
			bool hasEvent = s.canvas->pollEvent(event);
			if (event.type == sf::Event::Closed) s.canvas->close();
		});


	auto next = world.entity("SecondCircle")
		.set<canvas2d::Circle>({ 50, 34, 10 });

	auto e = world.entity("ThirdCircle")
		.set<canvas2d::Circle>({ 100, 200, 45 });

	return world.app().enable_rest().run();

}