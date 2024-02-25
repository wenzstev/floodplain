#include <flecs.h>
#include "factories.h"
#include <iostream>
namespace factories_module
{

	factories::factories(flecs::world& world)
	{
		world.import<flecs::units>();

		world.component<Stores>()
			.member<int32_t>("amount");

		world.component<StackCount>()
			.member<int32_t>("amount");

		world.component<Requirement>()
			.member(flecs::Entity, "resource")
			.member<int32_t>("amount");

		world.component<Requires>()
			.array<Requirement>(3);

		world.component<TimeToProduce>()
			.member<float, flecs::units::duration::Seconds>("value");

		world.component<Factory>()
			.on_add([](flecs::entity factory, Factory) {
				factory.add(FactoryState::Idle);
			 })
			.on_set(factory_init)
			.member(flecs::Entity, "recipe")
			.member(flecs::Entity, "inputs", 3)
			.member(flecs::Entity, "output");

		world.component<FactorySupply>()
			.member<int32_t>("required", 3)
			.member<int32_t>("collected", 3);

		world.component<FactoryProduction>()
			.member<float, flecs::units::Percentage>("value")
			.member<float, flecs::units::duration::Seconds>("duration")
			.member<int32_t>("max_stack");

		world.system<FactorySupply>("Collect")
			.with(FactoryState::WaitingForResources)
			.interval(1.0f)
			.each(collect_resources);


		world.system<FactoryProduction>("Produce")
			.with(FactoryState::Producing)
			.interval(0.1f)
			.each([](flecs::iter& it, size_t i, FactoryProduction& p)
				{
					p.value += it.delta_system_time() / p.duration;
					if (p.value >= 1) {
						p.value = 1;
						it.entity(i).add(FactoryState::TransferResource);
					}
				});

		world.system<FactorySupply, FactoryProduction, Stores>("Transfer")
			.term_at(3).second(flecs::Wildcard)
			.with(FactoryState::TransferResource)
			.interval(1.0f)
			.each([](flecs::entity factory, FactorySupply& s, FactoryProduction& p, Stores& out)
				{
					p.value = 0;

					Stores* depot = s.output.try_get();
					if (depot) {
						int32_t available_space = p.max_stack - depot->amount;
						if (out.amount > available_space) {
							depot->amount += available_space;
							out.amount -= available_space;
						}
						else {
							depot->amount += out.amount;
							out.amount = 0;
						}
					}

					Stores* store = &out;
					if (depot) {
						if (!p.max_stack || (depot->amount < p.max_stack)) {
							store = depot;
						}
					}

					if (store->amount < p.max_stack) {
						store->amount++;
						factory.add(FactoryState::WaitingForResources);
					}
					else {

					}
				});
	}

	

	void factories::factory_init(flecs::entity factory, Factory& config)
	{
		flecs::world world = factory.world();
		flecs::entity recipe = world.entity(config.recipe);

		recipe.get([&](const Requires& r) {
			flecs::entity output = world.entity(config.output);
			if (output)
			{
				if (!output.has<Stores>(recipe)) {
					std::cout << factory.path() << ": output doesn't provide resource" << recipe.path() << "\n";
					return;
				}
			}

			bool satisfied = true;
			for (int i = 0; i < MaxInputs; i++)
			{
				flecs::entity resource = world.entity(r.items[i].resource);
				flecs::entity input = world.entity(config.inputs[i]);
				if (!resource) {
					if (input)
					{
						std::cout << factory.path() << ": input connected to empty slot\n";
					}
					continue;
				}

				if (resource && !input)
				{
					satisfied = false;
					break;
				}

				if (!input.has<Stores>(resource)) {
					std::cout << factory.path() << ": input doesn't provide resource " << recipe.path() << "\n";
					satisfied = false;
					break;
				}
			}

			if (satisfied) {
				factory.add(FactoryState::WaitingForResources);

				factory.set([&](FactorySupply& s) {
					for (int i = 0; i < MaxInputs; i++) {
						flecs::entity resource = world.entity(r.items[i].resource);
						flecs::entity input = world.entity(config.inputs[i]);
						int32_t amount = r.items[i].amount;

						s.required[i] = amount;
						s.collected[i] = 0;

						if (!resource) {
							continue;
						}

						s.inputs[i] = input.get_ref<Stores>(resource);
					}

					if (output)
					{
						s.output = output.get_ref<Stores>(recipe);
					}
				});

				recipe.get([&](const TimeToProduce& ttp, const StackCount& sc) {
					factory.set<FactoryProduction>({ 0, ttp.value, sc.amount });
				});

				factory.add<Stores>(recipe);
			}

			}
		);	
		

	}



}
