#include <flecs.h>
#include <iostream>
#include <algorithm>
#include "grid.h"
#include "canvas2d.h"

using namespace std;


grid_module::grid_module(flecs::world& world)
{

	world.component<Grid>()
		.on_set(generate_grid)
		.member<flecs::entity>("Cell")
		.member<float>("X Count")
		.member<float>("Y Count");

	world.component<GridParams>()
		.member<float>("X Count")
		.member<float>("Y Count")
		.member<float>("X Spacing")
		.member<float>("Y Spacing")
		.member<float>("X Half")
		.member<float>("Y Half")
		.member<flecs::entity>("Cell");

}

void grid_module::generate_grid(flecs::entity grid, Grid& config) {
	flecs::world world = grid.world();

	GridParams params;
	params.xCount = max(1, config.x.count);
	params.yCount = max(1, config.y.count);


	params.xSpacing = max(0.001f, config.x.spacing);
	params.ySpacing = max(0.001f, config.y.spacing);
	

	params.xHalf = ((params.xCount - 1) / 2) * params.xSpacing;
	params.yHalf = ((params.yCount - 1) / 2) * params.ySpacing;

	flecs::entity oldScope = world.set_scope(grid);
	auto prefab = config.Prefab;

	prefab = params.prefab = get_prefab(world, prefab);

	if (!prefab) return;

	for (auto x = 0; x < params.xCount; x++)
	{
		for (auto y = 0; y < params.yCount; y++)
		{
			float xc = x * params.xSpacing - params.xHalf + config.origin.x;
			float yc = y * params.ySpacing - params.yHalf + config.origin.y;

			auto name = "Cell " + to_string(x) + " " + to_string(y);
			generate_tile(world, xc, yc, &params, name);
		}
	}

	world.set_scope(oldScope);


}

flecs::entity grid_module::generate_tile(flecs::world& world, float xc, float yc, const GridParams* params, std::string name)
{
	auto slot = params->prefab;

	flecs::entity instance = world.entity(name.c_str()).is_a(slot);
	
	instance.set<canvas2d::Vector2>({ xc, yc });
	return instance;
}



flecs::entity grid_module::get_prefab(flecs::world& world, flecs::entity prefab)
{
	if (!prefab) return world.entity();

	auto result = prefab;
	
	if (prefab.has<EcsScript>() && prefab.has<EcsComponent>())
	{
		result = world.entity();
		result.add(EcsPrefab);
		result.add(prefab);
	}
	return result;
}