#pragma once

#include <flecs.h>
#include <SFML/Graphics.hpp>
#include "transform.h"
#include "display.h"
#include "rendering.h"
#include <functional>

namespace canvas2d
{
	namespace input_processing
	{
		struct PixelLoc {
			int x, y;
		};

		struct MousePressed {
			sf::Mouse::Button pressedButton;
		};

		struct MouseReleased {
//			sf::Mouse::Button releasedButton;
		};

		struct InputState {
			sf::Mouse::Button LastPressed;
			sf::Mouse::Button LastReleased;
//			sf::Keyboard::Key LastKeyPressed;
			bool PressedKeys[sf::Keyboard::KeyCount] = { 0 };
			bool MouseButtonPressed[sf::Mouse::ButtonCount] = { 0 };
			PixelLoc CursorLocation;
		};


		/**
		 * @brief Determine if a point is within the bounds of a rectangle. 
		 *
		 * @param origin The origin of the rectangle.
		 * @param rect The bounds of the rectangle.
		 * @param pos The point to check if in bounds. 
		 * @return True if point is in bounds, false if it is not.
		 */
		bool is_inbounds(const transform::Position2& origin, const rendering::Rectangle& rect, const sf::Vector2f& pos);


		using ClickCallback = std::function<void(flecs::iter, size_t, canvas2d::input_processing::InputState, canvas2d::display::Screen&)>;
		/**
		*	@brief Set up an observer that fires when the mouse is clicked. When the mouse is clicked, the callback function will fire, with the
		*	provided input arguments.
		*	@param world The ecs world.
		*	@param callback The function that will be called when the mouse is clicked. 
		*/
		void register_click_action(flecs::world& world, ClickCallback callback);


		/**
		* @brief Check if the mouse is over a specific rectangle. 
		* @param world The ecs world.
		* @param input The InputState component that holds information about where the mouse is.
		* @param screen The display screen.
		* @returns The entity with the rectangle component the mouse is over, or null.
		*/
		flecs::entity get_mouseover_rect(flecs::world& world, canvas2d::input_processing::InputState& input, canvas2d::display::Screen& screen);


		struct module
		{
			module(flecs::world& world);
		private:
			static void process_sfml_events(flecs::iter& it, size_t i, canvas2d::display::Screen& screen, canvas2d::input_processing::InputState& inputState);
			static void process_camera_events(sf::Event evt, display::Screen& screen);
		};
	}
}