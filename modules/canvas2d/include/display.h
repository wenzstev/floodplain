#pragma once

#include <flecs.h>
#include <SFML/Graphics.hpp>

namespace canvas2d
{
	namespace display
	{
		struct ScreenDims
		{
			int xPixels;
			int yPixels;
			std::string screenName;
		};

		struct Screen
		{
			std::unique_ptr<sf::RenderWindow> canvas;
		};

		struct View
		{
			std::unique_ptr<sf::View> v;
		};

		struct ViewPos {};
		struct ViewScale {};

		struct module
		{
			module(flecs::world& world);
		private:
			static void setup_canvas(flecs::entity screen, ScreenDims& screenConfig);

		};
	}
}