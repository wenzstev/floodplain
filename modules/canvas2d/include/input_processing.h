#pragma once

#include <flecs.h>
#include <SFML/Graphics.hpp>
#include "transform.h"
#include "display.h"

namespace canvas2d
{
	namespace input_processing
	{
		struct PixelLoc {
			int x, y;
		};

		struct InputState {
			bool PressedKeys[sf::Keyboard::KeyCount] = { 0 };
			bool MouseButtonPressed[sf::Mouse::ButtonCount] = { 0 };
			PixelLoc CursorLocation;
		};

		struct module
		{
			module(flecs::world& world);
		private:
			static void process_sfml_events(flecs::iter& it, size_t i, canvas2d::display::Screen& screen, canvas2d::input_processing::InputState& inputState);
			static void process_input_events(sf::Event evt, InputState& inputState);
			static void process_camera_events(sf::Event evt, display::Screen& screen);
		};
	}
}