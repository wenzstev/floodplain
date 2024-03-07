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

	struct IsOn {};
	struct Agent
	{
		canvas2d::Color color;
	};

	flecs::world world;

	world.import<transform>();
	world.import<canvas2d>();
	world.import<grid_module>();
	world.import<flecs::monitor>();

	flecs::entity screenDims = world.entity()
		.set<canvas2d::ScreenDims>({ 1400, 1000, "Window in flecs" });

	auto gridPrefab = world.prefab("Rect")
		.set<canvas2d::Rectangle>({ 30, 30 })
		.add<canvas2d::DrawnIn, canvas2d::Background>();

	flecs::entity g = world.entity("Grid")
		.set<transform::Position2, transform::World>({ 700, 500 })
		.set<grid_module::Grid>({
			gridPrefab,
			{30, 32},
			{30, 32}
			});

	flecs::entity agentPrefab = world.prefab("Agent")
		.set<Agent>({ {100, 200, 100, 255} });


	auto f = world.filter<grid_module::Cell>();
	f.iter([](flecs::iter& it, grid_module::Cell* c)
		{
			for (auto i : it)
			{
				it.entity(i).set<canvas2d::Color>({ 250, 250, 250, 255 });
			}
		});



	world.system<Agent>("AgentMove")
		.each([](flecs::entity e, Agent& a)
			{
				if (rand() % 10 != 1) return;
				
				auto parent = e.parent();
				const grid_module::Cell *parentCell = parent.get<grid_module::Cell>();
				auto newCellIndex = rand() % 8;
				flecs::ref<grid_module::Cell> refCell = parentCell->neighbors[newCellIndex];
				if (refCell.try_get())
				{
					flecs::entity newCell = refCell.entity();
					e.remove(flecs::ChildOf, parent);
					e.add(flecs::ChildOf, newCell);
				}
			});

	world.system<Agent>("AgentReproduce")
		.each([agentPrefab](flecs::iter& it, size_t i, Agent& a)
			{
				if (rand() % 1000 != 1) return;

				auto world = it.world();
				auto newAgent = world.entity().is_a(agentPrefab);
				newAgent.set<canvas2d::Color>({ a.color.r, a.color.g, a.color.b, a.color.a });
				newAgent.set_override<transform::Position2, transform::Local>({ 0,0 });
				newAgent.set_override<transform::Position2, transform::World>({ 0,0 });
				auto agentEntity = it.entity(i);
				newAgent.child_of(agentEntity.parent());
			});

	world.system<grid_module::Cell, canvas2d::Color>("AgentDraw")
		.each([](flecs::entity e, grid_module::Cell& cell, canvas2d::Color& color)
			{
				std::cout << "iterating " << e.name() << "\n";
				float r = 0, g = 0, b = 0;
				int count = 0;
				e.children([&](flecs::entity child)
					{
						std::cout << "iterating children \n";
						auto* agent = child.get<Agent>();
						if (!agent) return;
						count++;
						r += agent->color.r;
						g += agent->color.g;
						b += agent->color.b;
					});
				if (count == 0) return;
				std::cout << r << " " << g << " " << b << "\n";
				color.r = r / count;
				color.g = g / count;
				color.b = b / count;
				color.a = 255;
			});


	flecs::entity cell = world.lookup("Grid::Cell 1 10");

	auto agent = world.entity().is_a(agentPrefab);
	agent.child_of(cell);




	return world.app().enable_rest().run();

}