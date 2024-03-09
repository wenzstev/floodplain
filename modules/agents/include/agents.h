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

	agents(flecs::world& world);
};