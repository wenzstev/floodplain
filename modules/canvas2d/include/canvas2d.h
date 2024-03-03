#pragma once

#include <flecs.h>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <variant>


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

	struct Vector2
	{
		float x; 
		float y;
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

	struct Color
	{
		float r;
		float g;
		float b;
		float a;
	};

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


	canvas2d(flecs::world& world);

private:
	static void init_window(flecs::entity screen, ScreenDims& screenConfig);
	static void draw_circle(flecs::entity e, Circle& circle);
	static void draw_rect(flecs::entity e, Rectangle& rectangle);

};

