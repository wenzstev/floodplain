#include "transform.h"

transform::transform(flecs::world& world) {
	world.component<transform::Position2>()
		.member<float>("X")
		.member<float>("Y");

	world.component<World>();
	world.component<Local>();

	world.component<Color>()
		.member<float>("Red")
		.member<float>("Green")
		.member<float>("Blue")
		.member<float>("Alpha");

	world.component<Rotation>()
		.member<float>("Angle");



	world.system<const transform::Position2, const transform::Position2, transform::Position2>()
		.term_at(1).second<Local>()
		.term_at(2).second<World>()
		.term_at(3).second<World>()
		.term_at(2).parent().cascade().optional()
		.iter(transform_components);
}

void transform::transform_components(flecs::iter& it, const Position2* p, const Position2* parent, Position2* worldPos)
{
	for (auto i : it)
	{
		worldPos[i].x = p[i].x;
		worldPos[i].y = p[i].y;
		if (parent)
		{
			worldPos[i].x += parent->x;
			worldPos[i].y += parent->y;
		}
	}
}