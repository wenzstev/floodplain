#include "canvas2d.h"
#include "display.h"
#include "input_processing.h"
#include "rendering.h"
#include "gui.h"
#include <flecs.h>

canvas2d::module::module(flecs::world& world)
{
	world.import<canvas2d::rendering::module>();
	world.import<canvas2d::display::module>();
	world.import<canvas2d::input_processing::module>();
	world.import<canvas2d::gui::module>();
}


