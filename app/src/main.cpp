#include <iostream>
#include <flecs.h>
#include <direct.h>
#include <filesystem>
#include <SFML/Graphics.hpp>
#include "factories.h"
#include "grid.h"
#include "flecs_game.h"
#include "flecs_components_transform.h"
#include "canvas2d.h"
#include "transform.h"



int main(int, char* []) {


	struct Agent {};

	struct IsOn {};


	flecs::world world;

	world.import<canvas2d>();

	flecs::entity screenDims = world.entity()
		.set<canvas2d::ScreenDims>({ 1400, 1000, "Window in flecs" });

	auto gridPrefab = world.prefab("Rect")
		.set<canvas2d::Rectangle>({ 50, 50 })
		.set<canvas2d::Color>({ 237, 225, 247, 255 });


	world.import<grid_module>();

	flecs::entity g = world.entity("Grid")
		.set<grid_module::Grid>({
			gridPrefab,
			{700, 450},
			{15, 55},
			{15, 55}
			});



	flecs::entity agent = world.entity("Agent")
		.add<Agent>()
		.set<canvas2d::Circle>({ 40 })
		.set<canvas2d::Color>({ 100, 100, 100, 255 })
		.set<canvas2d::Vector2>({ 0,0 });

	flecs::entity cell = world.lookup("Grid::Cell 1 1");

	std::cout << cell;

	agent.child_of(cell);

	world.import<transform>();

	auto ent = world.entity("Parent")
		.add<transform::Position2, transform::World>()
		.set<transform::Position2, transform::Local>({ 1, 1 });

	auto chi = world.entity("Child")
		.child_of(ent)
		.add<transform::Position2, transform::World>()
		.set<transform::Position2, transform::Local>({ 4, 3 });


	return world.app().enable_rest().run();

}