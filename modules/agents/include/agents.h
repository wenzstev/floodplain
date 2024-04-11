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


};