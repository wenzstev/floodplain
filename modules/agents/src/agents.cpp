#include <iostream>
#include <random>
#include <thread>

#include "agents.h"
#include "grid.h"


agents::agents(flecs::world& world)
{
	world.component<TotalPop>()
		.member<size_t>("Population");

	world.set<TotalPop>({ 0 });

	world.component<Agent>()
		.member<transform::Color>("Color");

	world.component<Age>()
		.member<int>("Age");

	world.component<CarryingCapacity>()
		.member<int>("Max Population");

	world.component<PopGained>();
	world.component<PopLost>();



	auto AgentReproduce = world.system<Agent>("AgentReproduce")
		.each([](flecs::iter& it, size_t i, Agent& a)
			{
				if (rand() % 5 != 1) return; // 5% chance to reproduce 
				auto world = it.world();

				auto newAgent = makeAgent(world, a);
				

				auto agentEntity = it.entity(i);
				newAgent.child_of(agentEntity.parent());
			})
		.add(agents::Main);


	auto CheckCC = world.system<CarryingCapacity>("CheckCC")
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
							destroyAgent(child);
						}
					});

			})
		.add(agents::Main);

	auto ChangeColor = world.system<Agent>("ChangeColor")
		//.multi_threaded()
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
			})
		.add(agents::Main);


	auto MergeAgents = world.system<>("MergeAgents")
		.with<CarryingCapacity>()
		//.multi_threaded()
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
			})
		.add(agents::Main);


	auto AgentMove = world.system<agents::Agent>("AgentMove")
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
					if (newCell.has<Impassable>()) return;
					e.remove(flecs::ChildOf, parent);
					e.add(flecs::ChildOf, newCell);
				}
			})
		.add(agents::Main);


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
			})
		.add(agents::Draw);

		world.system<transform::Color>("ImpassableDraw")
			.with<Impassable>()
			.each([](flecs::entity e, transform::Color& color)
				{
					color.r = 50;
					color.g = 50;
					color.b = 50;
					color.a = 255;
				})
			.add(agents::Draw);

	auto AgentAge = world.system<agents::Age>("AgentAge")
		.each([](flecs::entity e, agents::Age& a)
			{
				a.age++;
				if (a.age > 40) destroyAgent(e); 
			})
		.add(agents::Main);


}

flecs::entity agents::makeAgent(flecs::world& world, const agents::Agent& copy)
{
	auto agentPrefab = world.lookup("AgentPrefab");
	auto newAgent = world.entity();
	world.make_alive(newAgent);
	newAgent.set<agents::Age>({ 0 });
	newAgent.set<agents::Agent>({ { copy.color.r, copy.color.g, copy.color.b, copy.color.a } });
	newAgent.set<transform::Position2, transform::Local>({ 0,0 });
	newAgent.set<transform::Position2, transform::World>({ 0,0 });
	fireNewAgentEvent(world);
	return newAgent;
}

flecs::entity agents::makeRandomAgent(flecs::world& world, flecs::entity parent)
{
	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_int_distribution<> dis(0, 255);


	auto newAgent = world.entity();
	world.make_alive(newAgent);

	newAgent.set<agents::Age>({ 0 });
	newAgent.set<agents::Agent>({ {static_cast<float>(dis(gen)),static_cast<float>(dis(gen)), static_cast<float>(dis(gen)), 255 } });
	newAgent.set<transform::Position2, transform::Local>({ 0,0 });
	newAgent.set<transform::Position2, transform::World>({ 0,0 });
	newAgent.child_of(parent);

	agents::fireNewAgentEvent(world);
	return newAgent;

}



void agents::destroyAgent(flecs::entity& agent)
{
	agent.destruct();
	fireLostAgentEvent(agent.world());
}

void agents::fireNewAgentEvent(flecs::world& world)
{
	world.event<agents::PopGained>().id<agents::TotalPop>().entity(world.entity<agents::TotalPop>()).emit();
}

void agents::fireLostAgentEvent(flecs::world& world)
{
	world.event<agents::PopLost>().id<agents::TotalPop>().entity(world.entity<agents::TotalPop>()).emit();
}


void agents::startAgentSystems(flecs::world& world)
{
	flecs::filter<> f = world.filter_builder<>()
		.with(agents::Main)
		.with(flecs::Disabled)
		.build();

	world.defer([&] {
		f.each([](flecs::entity e)
			{
				e.remove(flecs::Disabled);
			});
		});


}

void agents::stopAgentSystems(flecs::world& world)
{
	flecs::filter<> f = world.filter_builder<>()
		.with(agents::Main)
		.build();

	world.defer([&] {
		f.each([](flecs::entity e)
			{
				e.add(flecs::Disabled);
			});
		});
}


void agents::clearAllAgents(flecs::world& world)
{
	flecs::filter<Agent> f = world.filter_builder<Agent>().build();
	stopAgentSystems(world);
	world.defer([&] {
		f.each([](flecs::entity e, Agent& a) 
			{
				e.destruct();
			});
		});
	startAgentSystems(world);
}

void agents::clearImpassableSquares(flecs::world& world)
{
	flecs::filter<> f = world.filter_builder<>().with<Impassable>().build();
	world.defer([&] {
		f.each([](flecs::entity e)
			{
				e.remove<Impassable>();
			});
		});
}