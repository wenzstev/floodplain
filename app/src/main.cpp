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

	world.system<agents::Agent>("AgentMove")
		.each([](flecs::entity e, agents::Agent& a)
			{
				if (rand() % 10 != 1) return;

				auto parent = e.parent();
				const grid_module::Cell* parentCell = parent.get<grid_module::Cell>();
				auto newCellIndex = rand() % 8;
				flecs::ref<grid_module::Cell> refCell = parentCell->neighbors[newCellIndex];
				if (refCell.try_get())
				{
					flecs::entity newCell = refCell.entity();
					e.remove(flecs::ChildOf, parent);
					e.add(flecs::ChildOf, newCell);
				}
			});

	world.system<grid_module::Cell, transform::Color>("AgentDraw")
		.each([](flecs::entity e, grid_module::Cell& cell, transform::Color& color)
			{
				float r = 0, g = 0, b = 0;
				int count = 0;
				e.children([&](flecs::entity child)
					{
						auto* agent = child.get<agents::Agent>();
						if (!agent) return;
						count++;
						r += agent->color.r;
						g += agent->color.g;
						b += agent->color.b;
					});
				if (count == 0)
				{
					color.r = 250;
					color.g = 250;
					color.b = 250;
					color.a = 255;
					return;
				}
				color.r = r / count;
				color.g = g / count;
				color.b = b / count;
				color.a = 255;
			});



	world.system<agents::Age>("Age")
		.each([](flecs::entity e, agents::Age& a)
			{
				a.age++;
				if (a.age > 40) e.destruct();
			});
	





	flecs::entity cellA = world.lookup("Grid::Cell 1 10");
	flecs::entity cellB = world.lookup("Grid::Cell 20 1");
	flecs::entity cellC = world.lookup("Grid::Cell 25 25");

	auto agentA = world.entity().is_a(agentPrefab).set<agents::Agent>({ {100, 200, 100, 255} });
	auto agentB = world.entity().is_a(agentPrefab).set<agents::Agent>({ {200, 100, 100, 255} });
	auto agentC = world.entity().is_a(agentPrefab).set<agents::Agent>({ {100, 100, 200, 255} });


	agentA.child_of(cellA);
	agentB.child_of(cellB);
	agentC.child_of(cellC);


	world.set_threads(12);
	return world.app().enable_rest().run();

}