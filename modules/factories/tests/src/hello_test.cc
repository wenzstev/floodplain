#include <gtest/gtest.h>
#include <flecs.h>
#include "factories.h"

class FactoriesTest : public ::testing::Test 
{
	protected:
		flecs::world ecs;
		flecs::entity e;

		void SetUp() override {
			ecs.import<factories_module::factories>();
			e = ecs.entity("test_factory");

		}
};

TEST_F(FactoriesTest, AllResourcesSufficient) {
	auto store = ecs.entity("test_store");
	auto testTag = ecs.entity();
	struct stuff {};

	store.add<factories_module::factories::Stores, stuff>();
	auto storeRef = store.get_ref<factories_module::factories::Stores>();

	e.add(factories_module::factories::FactoryState::WaitingForResources);
	e.set<factories_module::factories::FactorySupply>({
		{1, 0, 0},
		{1, 0, 0},
		{storeRef, storeRef, storeRef},
		storeRef
		});

	ecs.progress(1.0f);

	const factories_module::factories::FactoryState* currentState = e.get<factories_module::factories::FactoryState>();

	ASSERT_EQ(*currentState, factories_module::factories::FactoryState::Producing);
}

TEST_F(FactoriesTest, ResourceDeficiency) {
	auto store = ecs.entity("test_store");

	struct stuff {};
	auto storeStuff = ecs.entity().add<stuff>();

	store.set<factories_module::factories::Stores, stuff>({10});
	auto storeRef = store.get_ref<factories_module::factories::Stores, stuff>();
	e.add(factories_module::factories::FactoryState::WaitingForResources);
	e.set<factories_module::factories::FactorySupply>({
		{1, 0, 0},
		{0, 0, 0},
		{storeRef, storeRef, storeRef},
		storeRef
		});

	ecs.progress(1.1f);

	auto testMod = e.get<factories_module::factories::FactorySupply>();
	auto testStore = store.get<factories_module::factories::Stores, stuff>();

	ASSERT_EQ(testStore->amount, 9);

}