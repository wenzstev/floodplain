#pragma once
#include <flecs.h>

struct transform 
{
	struct Position2 {
		float x, y;
	};

	struct World {};
	struct Local {};


	transform(flecs::world& world);

	private:
		static void transform_components(flecs::iter& it, const Position2* p, const Position2* parent, Position2* worldPos);
};