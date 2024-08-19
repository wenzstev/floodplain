#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <SFML/Graphics.hpp>
#include <queue>

#include <TGUI/TGUI.hpp>
#include "canvas2d.h"
#include "input_processing.h"



TEST(Canvas2dTest, CanRegisterClickAction)
{
	flecs::world world;
	world.import<canvas2d::module>();

	auto screen = world.entity("TestScreen").add<canvas2d::display::Screen>();

	bool didTrigger = false;

	auto TestFunc = [&world, &didTrigger](flecs::iter& it, size_t i, canvas2d::input_processing::InputState& input, canvas2d::display::Screen& screen)
		{
			didTrigger = true;
		};



	canvas2d::input_processing::register_click_action(world, TestFunc);

	auto inputState = world.entity<canvas2d::input_processing::InputState>();
	
	world.event<canvas2d::input_processing::MousePressed>().id<canvas2d::input_processing::InputState>().entity(inputState).emit();
	world.app().enable_rest().run();
	ASSERT_TRUE(didTrigger) << "Callback was not fired!";

}

