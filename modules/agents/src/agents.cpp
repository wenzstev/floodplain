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
		.multi_threaded()
		.each([](flecs::iter& it, size_t i, Agent& a)
			{
				if (rand() % 25 != 1) return;
				auto world = it.world();

				auto agentPrefab = world.lookup("AgentPrefab");
				std::cout << "Agent prefab: " << agentPrefab << "\n";
				auto newAgent = world.entity();
				world.make_alive(newAgent);
				std::cout << "New agent: " << newAgent << "\n";
				std::cout << "Readonly? " << world.is_readonly() << "\n";
				std::cout << "New agent is alive? " << newAgent.is_alive() << "\n";
				newAgent.set<agents::Age>({ 0 });
				newAgent.set<agents::Agent>({ { a.color.r, a.color.g, a.color.b, a.color.a } });
				newAgent.set<transform::Position2, transform::Local>({ 0,0 });
				newAgent.set<transform::Position2, transform::World>({ 0,0 });
				auto agentEntity = it.entity(i);
				newAgent.child_of(agentEntity.parent());
			});

	world.system<CarryingCapacity>("CheckCC")
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
		.each([](flecs::entity e, Agent& a)
			{
				transform::Color agentColor = a.color;
				auto modifyColor = [](float c) -> float {
					std::random_device rd;
					std::mt19937 gen(rd());
					std::uniform_real_distribution<float> dis(-1.0, 1.0);
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
}