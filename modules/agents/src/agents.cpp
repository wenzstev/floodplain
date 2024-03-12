#include <iostream>
#include <random>
#include <thread>

#include "agents.h"
#include "grid.h"

agents::agents(flecs::world& world)
{
	world.component<Agent>()
		.member<transform::Color>("Color");

	world.component<Age>()
		.member<int>("Age");

	world.component<CarryingCapacity>()
		.member<int>("Max Population");


	world.system<Agent>("AgentReproduce")
		.each([](flecs::iter& it, size_t i, Agent& a)
			{
				if (rand() % 5 != 1) return;
				auto world = it.world();

				auto agentPrefab = world.lookup("AgentPrefab");
				auto newAgent = world.entity();
				world.make_alive(newAgent);
				newAgent.set<agents::Age>({ 0 });
				newAgent.set<agents::Agent>({ { a.color.r, a.color.g, a.color.b, a.color.a } });
				newAgent.set<transform::Position2, transform::Local>({ 0,0 });
				newAgent.set<transform::Position2, transform::World>({ 0,0 });
				auto agentEntity = it.entity(i);
				newAgent.child_of(agentEntity.parent());
			});

	world.system<CarryingCapacity>("CheckCC")
		.multi_threaded()
		.each([](flecs::entity e, const CarryingCapacity& cc)
			{
				int count = 0;
				e.children([&](flecs::entity child)
					{
						auto* agent = child.get<Agent>();
						if (!agent) return;
						count++;
						if (count > cc.maxPop)
						{
							child.mut(e).destruct();
						}
					});

			});

	world.system<Agent>("ChangeColor")
		.multi_threaded()
		.each([](flecs::entity e, Agent& a)
			{
				transform::Color agentColor = a.color;
				auto modifyColor = [](float c) -> float {
					static std::random_device rd;
					static std::mt19937 gen(rd());
					static std::uniform_real_distribution<float> dis(-1.0, 1.0);
					float randVal = dis(gen);

					return c + randVal;
					};

				e.set<Agent>({
					modifyColor(a.color.r),
					modifyColor(a.color.g),
					modifyColor(a.color.b),
					255
					});
			});

	auto mergeAgents = world.system<>("MergeAgents")
		.with<CarryingCapacity>()
		.multi_threaded()
		.each([](flecs::entity e)
			{
				float r = 0, g = 0, b = 0;
				int count = 0;
				e.children([&](flecs::entity child)
					{ 
						auto agent = child.get<Agent>();
						if (!agent) return;
						r += agent->color.r;
						g += agent->color.g;
						b += agent->color.b;
						count++;
					});
				if (count == 0) return;
				r = r / count;
				g = g / count;
				b = b / count;

				e.children([&](flecs::entity child)
					{
						auto agent = child.get_mut<Agent>();
						if (!agent) return;

						auto prevR = agent->color.r;
						auto prevG = agent->color.g;
						auto prevB = agent->color.b;

						auto dr = r - agent->color.r;
						auto dg = g - agent->color.g;
						auto db = b - agent->color.b;

						float change = 0.01;

						auto cr = dr * change;
						auto cg = dg * change;
						auto cb = db * change;

						agent->color.r = cr + prevR;
						agent->color.g = cg + prevG;
						agent->color.b = cb + prevB;
					});
			});


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

	world.system<transform::Color>("AgentDraw")
		.with<CarryingCapacity>()
		.each([](flecs::entity e, transform::Color& color)
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

}