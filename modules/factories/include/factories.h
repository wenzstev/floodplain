#ifndef FACTORIES_H
#define FACTORIES_H

#include <flecs.h>


namespace factories_module {

	struct factories {
		static constexpr int MaxInputs = 3;

		struct StackCount {
			int32_t amount;
		};

		struct Stores {
			int32_t amount;
		};

		struct Requirement {
			flecs::entity_t resource;
			int32_t amount;
		};

		struct Requires {
			Requirement items[MaxInputs];
		};

		struct TimeToProduce {
			float value;
		};

		struct Factory {
			flecs::entity_t recipe;
			flecs::entity_t inputs[MaxInputs];
			flecs::entity_t output;
		};

		struct FactorySupply {
			int32_t required[MaxInputs];
			int32_t collected[MaxInputs];
			flecs::ref<Stores> inputs[MaxInputs];
			flecs::ref<Stores> output;
		};

		struct FactoryProduction {
			float value;
			float duration;
			int32_t max_stack;
		};

		enum class FactoryState {
			Idle,
			WaitingForResources,
			Producing,
			TransferResource
		};

		factories(flecs::world& world);
	private:
		static void factory_init(flecs::entity factory, Factory& config);
		static void collect_resources(flecs::entity factory, FactorySupply& s);
		static bool check_resource(FactorySupply& supply, int32_t curResource);
		static void empty_supply(FactorySupply& supply);
	};

}
#endif