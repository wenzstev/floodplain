#pragma once

#include "transform.h"

struct agents
{
	struct Agent
	{
		transform::Color color;
	};

	struct Age
	{
		int age;
	};

	struct CarryingCapacity
	{
		int maxPop;
	};


	struct TotalPop {
		size_t value;
	};

	struct PopGained {};
	struct PopLost {};

	agents(flecs::world& world);


	static flecs::entity makeAgent(flecs::world& world, const agents::Agent& parent);
	static void destroyAgent(flecs::entity& agent);
	static void fireLostAgentEvent(flecs::world& world);
	static void fireNewAgentEvent(flecs::world& world);


};