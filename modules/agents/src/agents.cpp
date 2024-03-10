#include <iostream>
#include <random>

#include "agents.h"

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

	world.system<CarryingCapacity>("MergeAgents")
		.multi_threaded()
		.each([](flecs::entity e, CarryingCapacity& c)
			{
				float r, g, b;
				int count;
				e.children([&](flecs::entity child)
					{ 
						auto agent = child.get<Agent>();
						if (!agent) return;
						r += agent->color.r;
						g += agent->color.g;
						b += agent->color.b;
						count++;
					});
				r = r / count;
				g = g / count;
				b = b / count;

				e.children([&](flecs::entity child)
					{
						auto agent = child.get<Agent>();
						if (!agent) return;
						auto dr = r - agent->color.r;
						auto dg = g - agent->color.g;
						auto db = b - agent->color.b;

						auto change = 0.05;

						auto cr = dr * change;
						auto cg = dg * change;
						auto cb = db * change;

						child.mut(e).set<Agent>({{ agent->color.r * cr, agent->color.g * cg, agent->color.b * cb, 255 }});
					});
			});
}