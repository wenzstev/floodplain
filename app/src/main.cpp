#include <iostream>
#include <flecs.h>
#include <direct.h>
#include <thread>
#include <random>
#include <filesystem>
#include <SFML/Graphics.hpp>
#include "grid.h"
#include "canvas2d.h"
#include "transform.h"
#include "agents.h"


int main(int, char* []) {

	flecs::world world;

	world.import<transform>();
	world.import<canvas2d>();
	world.import<grid_module>();
	world.import<agents>();
	world.import<flecs::monitor>();

	flecs::entity screenDims = world.entity()
		.set<canvas2d::ScreenDims>({ 1400, 1000, "Window in flecs" });

	auto gridPrefab = world.prefab("Rect")
		.set<canvas2d::Rectangle>({ 30, 30 })
		.add<canvas2d::DrawnIn, canvas2d::Background>()
		.set<agents::CarryingCapacity>({ 20});

	flecs::entity g = world.entity("Grid")
		.set<transform::Position2, transform::World>({ 700, 500 })
		.set<grid_module::Grid>({
			gridPrefab,
			{30, 32},
			{30, 32}
			});

	flecs::entity agentPrefab = world.prefab("AgentPrefab")
		.set<agents::Age>({ 0 });

	world.defer_begin();
	auto f = world.filter<grid_module::Cell>();
	f.iter([](flecs::iter& it, grid_module::Cell* c)
		{
			for (auto i : it)
			{
				it.entity(i).set<transform::Color>({ 250, 250, 250, 255 });
			}
		});
	world.defer_end();


	flecs::entity cellA = world.lookup("Grid::Cell 1 10");
	flecs::entity cellB = world.lookup("Grid::Cell 20 1");
	flecs::entity cellC = world.lookup("Grid::Cell 25 25");

	auto agentA = world.entity().is_a(agentPrefab).set<agents::Agent>({ {100, 200, 100, 255} });
	auto agentB = world.entity().is_a(agentPrefab).set<agents::Agent>({ {200, 100, 100, 255} });
	auto agentC = world.entity().is_a(agentPrefab).set<agents::Agent>({ {100, 100, 200, 255} });


	agentA.child_of(cellA);
	agentB.child_of(cellB);
	agentC.child_of(cellC);

	
	auto camera = world.entity();
	camera.set<canvas2d::ViewPos, transform::Position2>({ 350, 300 });
	camera.set<canvas2d::ViewScale, transform::Position2>({ 300, 200 });
	camera.add<canvas2d::View>();

	flecs::entity wpressed = world.entity();
	wpressed.add(sf::Keyboard::W);


	world.system<sf::Keyboard::Key>()
		.term_at(1).second(flecs::Wildcard)
		.iter([](flecs::iter& it, sf::Keyboard::Key* key)
			{
				for (auto i : it)
				{
					auto entity = it.entity(i);
					auto key = it.pair(1).second();
					const sf::Keyboard::Key* k = key.get<sf::Keyboard::Key>();
					if (sf::Keyboard::isKeyPressed(*k))
					{
						std::cout << k << " is pressed! \n";
					}
				}

			});

	world.set_threads(12);
	return world.app().enable_rest().run();

}