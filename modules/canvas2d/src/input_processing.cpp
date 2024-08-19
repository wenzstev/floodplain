#include <flecs.h>
#include "input_processing.h"
#include "display.h"
#include "gui.h"

using namespace canvas2d;

template <typename T>
void fire_event(flecs::world& world, flecs::entity InputEntity) {
	world.event<T>().id<canvas2d::input_processing::InputState>().entity(InputEntity).emit();
}

input_processing::module::module(flecs::world& world)
{
	world.component<InputState>();
	world.add<InputState>();
	world.component<PixelLoc>()
		.member<int>("X")
		.member<int>("Y");

	world.system<display::Screen, InputState>()
		.kind(flecs::OnLoad)
		.term_at(1).singleton()
		.term_at(2).singleton()
		.each(process_sfml_events);


}

void canvas2d::input_processing::module::process_sfml_events(flecs::iter& it, size_t i, display::Screen& screen, InputState& inputState)
{
	sf::Event event;
	auto guiSingleton = it.world().get<gui::GUI>();
	while (screen.canvas->pollEvent(event))
	{
		if (guiSingleton) guiSingleton->gui->handleEvent(event);
		auto InputEntity = it.world().entity<InputState>();
		auto world = it.world();

		

		switch (event.type)
		{
		case sf::Event::Closed:
			screen.canvas->close();
			break;
		case sf::Event::KeyPressed:
			inputState.PressedKeys[event.key.code] = true;
			break;
		case sf::Event::KeyReleased:
			inputState.PressedKeys[event.key.code] = false;
			break;
		case sf::Event::MouseButtonPressed:
			inputState.MouseButtonPressed[event.mouseButton.button] = true;
			inputState.LastPressed = event.mouseButton.button;
			fire_event<MousePressed>(world, InputEntity);
			break;
		case sf::Event::MouseButtonReleased:
			inputState.MouseButtonPressed[event.mouseButton.button] = false;
			inputState.LastReleased = event.mouseButton.button;
			fire_event<MouseReleased>(world, InputEntity);
			break;
		case sf::Event::MouseMoved:
			inputState.CursorLocation.x = event.mouseMove.x;
			inputState.CursorLocation.y = event.mouseMove.y;
			break;
		default:
			break;
		}
	}
}



bool input_processing::is_inbounds(const transform::Position2& origin, const rendering::Rectangle& rect, const sf::Vector2f& pos)
{
	return pos.x >= origin.x && pos.x <= (origin.x + rect.width) &&
		pos.y >= origin.y && pos.y <= (origin.y + rect.height);
}


void input_processing::register_click_action(flecs::world& world, ClickCallback callback)
{
	world.observer<canvas2d::input_processing::InputState, canvas2d::display::Screen>()
		.term_at(1).singleton()
		.term_at(2).singleton()
		.event<canvas2d::input_processing::MousePressed>()
		.each(callback);
}

flecs::entity input_processing::get_mouseover_rect(flecs::world& world, canvas2d::input_processing::InputState& input, canvas2d::display::Screen& screen)
{
	flecs::filter<canvas2d::rendering::Rectangle, transform::Position2> q = world.filter_builder<canvas2d::rendering::Rectangle, transform::Position2>()
		.term_at(2).second<transform::World>()
		.build();

	auto mousePos = input.CursorLocation;
	sf::Vector2i point = { mousePos.x, mousePos.y };
	auto mouseWorldPos = screen.canvas->mapPixelToCoords(point);
	return q.find([&mouseWorldPos](canvas2d::rendering::Rectangle& r, transform::Position2& p)
		{
			return canvas2d::input_processing::is_inbounds(p, r, mouseWorldPos);
		});
}