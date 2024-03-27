#pragma once

#include <flecs.h>
#include <SFML/Graphics.hpp>
#include "transform.h"

namespace canvas2d
{
	struct module
	{
		module(flecs::world& world);
	};
}