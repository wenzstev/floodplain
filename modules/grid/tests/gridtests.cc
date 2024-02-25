#include <gtest/gtest.h>
#include <flecs.h>
#include "grid.h"


TEST(GridTest, CanInstantiateGrid)
{
	flecs::world world;


	auto testPrefab = world.prefab("prefab");

	world.import<grid_module>();

	auto grid = world.entity("GridTest").set<grid_module::Grid>({ 
		testPrefab, 
		{2, 1.0f}, 
		{2, 1.0f} 
	});


	flecs::query<grid_module::Position2> q = world.query<grid_module::Position2>();

	flecs::entity e = q.find([](grid_module::Position2 & p) {
		return p.x == 1;
	});

	int count = 0;
	bool allPrefabs = true;

	q.each([&count, &allPrefabs, &testPrefab](flecs::entity e, grid_module::Position2& p) {
		count++;
		allPrefabs = e.has(flecs::IsA, testPrefab);
		});

	ASSERT_EQ(true, allPrefabs);
	ASSERT_EQ(4, count);
}