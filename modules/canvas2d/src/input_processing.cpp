#include <flecs.h>
#include "input_processing.h"
#include "display.h"
#include "gui.h"

using namespace canvas2d;

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

		switch (event.type)
		{
		case sf::Event::Closed:
			screen.canvas->close();
			break;
		case sf::Event::KeyPressed:
		case sf::Event::KeyReleased:
		case sf::Event::MouseButtonPressed:
		case sf::Event::MouseButtonReleased:
		case sf::Event::MouseMoved:
			process_input_events(event, inputState);
			break;
		default:
			break;
		}
	}
}

void input_processing::module::process_input_events(sf::Event event, InputState& inputState)
{
	switch (event.type)
	{
	case sf::Event::KeyPressed:
		inputState.PressedKeys[event.key.code] = true;
		break;
	case sf::Event::KeyReleased:
		inputState.PressedKeys[event.key.code] = false;
		break;
	case sf::Event::MouseButtonPressed:
		inputState.MouseButtonPressed[event.mouseButton.button] = true;
		break;
	case sf::Event::MouseButtonReleased:
		inputState.MouseButtonPressed[event.mouseButton.button] = false;
		break;
	case sf::Event::MouseMoved:
		inputState.CursorLocation.x = event.mouseMove.x;
		inputState.CursorLocation.y = event.mouseMove.y;
		break;
	default:
		break;
	}
}