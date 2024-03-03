#pragma once


#include "canvas2d.h"

struct grid_module
{
	struct GridParams {
		int xCount;
		int yCount;
		float xSpacing;
		float ySpacing;
		float xHalf;
		float yHalf;
		flecs::entity prefab;
	};

	struct GridCoord {
		int32_t count;
		float spacing;
	};

	struct Grid {
		flecs::entity Prefab;
		canvas2d::Vector2 origin;
		GridCoord x;
		GridCoord y;
	};

	grid_module(flecs::world& world);

	private:
		static void generate_grid(flecs::entity grid, Grid& config);
		static void create_cell(flecs::world& world, const Grid* grid);
		static flecs::entity generate_tile(flecs::world& world, float xc, float yc, const GridParams* params, std::string name);
		static flecs::entity get_prefab(flecs::world& world, flecs::entity prefab);
};
