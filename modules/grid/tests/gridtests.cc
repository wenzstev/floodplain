#include <gtest/gtest.h>
#include <flecs.h>
#include <algorithm>
#include "grid.h"
#include "canvas2d.h"


TEST(GridTest, CanInstantiateGrid)
{
	flecs::world world;


	auto testPrefab = world.prefab("prefab");

	world.import<grid_module>();

	auto grid = world.entity("GridTest").set<grid_module::Grid>({ 
		testPrefab, 
		{0, 0},
		{2, 1.0f}, 
		{2, 1.0f} 
	});


	flecs::query<canvas2d::Vector2> q = world.query<canvas2d::Vector2>();

	int count = 0;
	bool allPrefabs = true;

	q.each([&count, &allPrefabs, &testPrefab](flecs::entity e, canvas2d::Vector2& p) {
		count++;
		allPrefabs = e.has(flecs::IsA, testPrefab);
		});

	ASSERT_EQ(true, allPrefabs);
	ASSERT_EQ(4, count);
}

TEST(GridTest, CanCenterGrid)
{
	flecs::world world;
	auto testPrefab = world.prefab("prefab");

	world.import<grid_module>();

	auto grid = world.entity("GridTest").set<grid_module::Grid>({
		testPrefab,
		{100, 100},
		{2, 1.0f},
		{2, 1.0f}
		});


	flecs::query<canvas2d::Vector2> q = world.query<canvas2d::Vector2>();

	std::vector<canvas2d::Vector2> expectedLocations = {
		{100, 100},
		{100, 101},
		{101, 100},
		{101, 101}
	};

	std::vector<canvas2d::Vector2> actualLocations;

	q.each([&](flecs::entity e, canvas2d::Vector2& v)
		{
			actualLocations.push_back(v);
		});

	ASSERT_EQ(actualLocations.size(), expectedLocations.size());

	for (const auto& loc : actualLocations) {
		auto it = std::find_if(expectedLocations.begin(), expectedLocations.end(), [&](const canvas2d::Vector2& expectedLoc) {
			return loc.x == expectedLoc.x && loc.y == expectedLoc.y;
			});
		ASSERT_TRUE(it != expectedLocations.end()) << "Entity found at unexpected location: (" << loc.x << ", " << loc.y << ")";
	}

}