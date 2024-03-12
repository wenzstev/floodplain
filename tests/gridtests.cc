#include <gtest/gtest.h>
#include <flecs.h>
#include <algorithm>
#include "grid.h"
#include "transform.h"


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

	flecs::filter<> f = world.filter_builder()
		.term<transform::Position2, transform::Local>()
		.term<grid_module::Cell>()
		.build();

	int count = 0;
	bool allPrefabs = true;

	f.each([&count, &allPrefabs, &testPrefab](flecs::iter& it, size_t index) {
		count++;
		allPrefabs = it.entity(index).has(flecs::IsA, testPrefab);
		});

	ASSERT_EQ(true, allPrefabs);
	ASSERT_EQ(4, count);
}

TEST(GridTest, CanCreateCells)
{
	flecs::world world; 

	auto testPrefab = world.prefab("prefab");

	world.import<grid_module>();

	auto grid = world.entity("GridTest").set<grid_module::Grid>({
		testPrefab,
		{3, 1.0f},
		{3, 1.0f}
		});

	auto center = world.lookup("GridTest::Cell 1 1");

	const grid_module::Cell *cell = center.get<grid_module::Cell>();

	auto neighbors = cell->neighbors;
	std::string expectedVals[8] = { "1 0", "2 0", "2 1", "2 2", "1 2", "0 2", "0 1", "0 0" };

	for (auto i = 0; i < 8; i++)
	{
		ASSERT_NE(neighbors[i].entity(), 0);
		std::stringstream ss;
		ss << "Cell " << expectedVals[i];
		ASSERT_EQ(ss.str().compare(neighbors[i].entity().name()), 0);

	}



}