#include <flecs.h>
#include <iostream>
#include <SFML/Graphics.hpp>


struct canvas2d
{
	struct Circle
	{
		float x;
		float y;
		float r;
	};

	struct ScreenDims
	{
		int xPixels;
		int yPixels;
		std::string screenName;
	};

	struct Screen
	{
		sf::RenderWindow* canvas;
	};

	canvas2d(flecs::world& world);

private:
	static void init_window(flecs::entity screen, ScreenDims& screenConfig);
};
