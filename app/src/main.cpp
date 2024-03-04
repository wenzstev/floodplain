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

	flecs::world world;

	world.import<transform>();
	world.import<canvas2d>();
	world.import<grid_module>();

	flecs::entity screenDims = world.entity()
		.set<canvas2d::ScreenDims>({ 1400, 1000, "Window in flecs" });

	auto gridPrefab = world.prefab("Rect")
		.set<canvas2d::Rectangle>({ 50, 50 })
		.set<canvas2d::Color>({ 237, 225, 247, 255 })
		.add<canvas2d::DrawnIn, canvas2d::Background>();

	flecs::entity g = world.entity("Grid")
		.set<transform::Position2, transform::World>({700, 500})
		.set<grid_module::Grid>({
			gridPrefab,
			{15, 55},
			{15, 55}
			});

	flecs::entity agent = world.entity("Agent")
		.add<Agent>()
		.set<canvas2d::Circle>({ 25 })
		.set<canvas2d::Color>({ 100, 100, 100, 255 })
		.add<canvas2d::DrawnIn, canvas2d::Main>()
		.set<transform::Position2, transform::Local>({ 0,0 })
		.add<transform::Position2, transform::World>();

	world.system<Agent>("AgentMove")
		.each([](flecs::entity e, Agent& a)
			{
				auto parent = e.parent();
				std::string name = parent.name();

			});


	flecs::entity cell = world.lookup("Grid::Cell 1 10");

	std::cout << cell;

	agent.child_of(cell);




	return world.app().enable_rest().run();

}