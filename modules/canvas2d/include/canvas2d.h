#pragma once

#include <flecs.h>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <variant>
#include "transform.h"


struct canvas2d
{
	struct Circle
	{
		float radius;
	};

	struct Rectangle
	{
		float width;
		float height;
	};

	struct Rotation
	{
		float angle;
	};

	struct Scale
	{
		float scale;
	};

	struct OriginatesAt {};
	struct ScalesTo {};



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

	struct SFMLEvent {
		sf::Event* event;
	};


	struct DrawnIn {};
	struct Background {};
	struct Main {};
	struct Foreground {};


	canvas2d(flecs::world& world);

private:
	static void init_window(flecs::entity screen, ScreenDims& screenConfig);
	static void setup_canvas(flecs::world& world, ScreenDims& screenConfig);
	static void setup_draw_phases(flecs::world& world);

};

