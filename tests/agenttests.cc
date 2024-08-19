# include <gtest/gtest.h>
# include "agents.h"
# include <flecs.h>



TEST(AgentTest, CanPauseAgentSim)
{
	flecs::world world;
	world.import<agents>();

	agents::stopAgentSystems(world);

	flecs::filter<>f = world.filter_builder()
		.with(agents::Main)
		.with(flecs::Disabled)
		.build();

	bool allAreDisabled = true;
	f.each([&](flecs::entity e)
		{
			allAreDisabled = e.has(flecs::Disabled);
		});

	ASSERT_TRUE(allAreDisabled) << "Not every system is disabled!";
}

TEST(AgentTest, CanRestartAgentSim)
{
	flecs::world world;
	world.import<agents>();

	agents::stopAgentSystems(world);
	agents::startAgentSystems(world);

	flecs::filter<>f = world.filter_builder()
		.with(agents::Main)
		.build();

	bool allAreEnabled = true;

	f.each([&](flecs::entity e)
		{
			allAreEnabled = !e.has(flecs::Disabled);
		});

	ASSERT_TRUE(allAreEnabled) << "Not every system is enabled!";
}

TEST(AgentTest, CanClearImpassableSquares)
{
	flecs::world world;
	world.import<agents>();

	flecs::entity e = world.entity().add<agents::Impassable>();

	agents::clearImpassableSquares(world);

	ASSERT_TRUE(!e.has<agents::Impassable>()) << "Impassable tag was not removed from agent!";
}


// test that agent can move

// test that agent can change color 

// test that agent can have color averages 

// test that carrying capacity works 

// test that color averages work to display on a cell

