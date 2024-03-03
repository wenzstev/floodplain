#include <flecs.h>

struct transform {
	struct Position2 {
		float x, y;
	};

	struct World {};
	struct Local {};


	transform(flecs::world& world);

	
};