#include <flecs.h>
#include "factories.h"
#include <iostream>

namespace factories_module {
	void factories::collect_resources(flecs::entity factory, FactorySupply& supply) {
		bool hasResources = true;


		for (std::size_t curResource = 0; curResource < MaxInputs; curResource++)
		{
			if (!check_resource(supply, curResource)) {
				hasResources = false;
				break;
			}
		}

		if (hasResources) {
			empty_supply(supply);
			factory.add(FactoryState::Producing);
		}
	}

	bool factories::check_resource(FactorySupply& supply, int32_t curResource)
	{
		int32_t requiredAmount = supply.required[curResource] - supply.collected[curResource];

		if (requiredAmount <= 0) return true;

		Stores* curStore = supply.inputs[curResource].get();
		if (curStore->amount >= requiredAmount) {
			supply.collected[curResource] += requiredAmount;
			curStore->amount -= requiredAmount;
			return true;
		}

		supply.collected[curResource] += curStore->amount;
		curStore->amount = 0;
		return false;
	};

	void factories::empty_supply(FactorySupply& supply) {
		std::fill_n(supply.collected, MaxInputs, 0);
	}
}