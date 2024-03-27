#include <flecs.h>
#include "display.h"
#include "transform.h"
#include <iostream>

using namespace canvas2d;

display::module::module(flecs::world& world)
{
	world.component<ScreenDims>()
		.on_set(setup_canvas)
		.member<int>("width")
		.member<int>("height")
		.member<std::string>("name");

	world.component<Screen>()
		.member<std::unique_ptr<sf::RenderWindow>>("canvas");

	world.component<View>()
		.member<std::unique_ptr<sf::View>>("View");

	world.component<ViewPos>();
	world.component<ViewScale>();

	// using observer because I'm not sure how to do hooks with two components
	world.observer<transform::Position2, transform::Position2, Screen, View>()
		.term_at(1).second<ViewPos>()
		.term_at(2).second<ViewScale>()
		.term_at(3).singleton()
		.term_at(4).singleton()
		.event(flecs::OnSet)
		.each([](flecs::iter& it, size_t i, transform::Position2& pos, transform::Position2& scale, Screen& screen, View& view)
			{

				std::unique_ptr<sf::View> viewPtr(new sf::View(
					sf::Vector2f(pos.x, pos.y),
					sf::Vector2f(scale.x, scale.y)));

				view.v = std::move(viewPtr);
				screen.canvas->setView(*view.v);
			});


}

void display::module::setup_canvas(flecs::entity screen, ScreenDims& screenConfig)
{
	flecs::world world = screen.world();
	std::unique_ptr<sf::RenderWindow> window(new sf::RenderWindow(sf::VideoMode(screenConfig.xPixels, screenConfig.yPixels), screenConfig.screenName));
	world.set<Screen>({ std::move(window) });
	world.add<View>();

	world.system<Screen>()
		.kind(flecs::OnStore)
		.each([](flecs::entity e, Screen& s)
			{
				s.canvas->display();
			});


	world.system<Screen>()
		.kind(flecs::PreUpdate)
		.each([](flecs::entity e, Screen& s)
			{
				s.canvas->clear(sf::Color::Black);
			});



}

