#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <variant>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include "canvas2d.h"
#include "display.h"
#include "gui.h"


void simulateClick(tgui::Button::Ptr button) {
	tgui::Event event;
	event.type = tgui::Event::Type::MouseButtonPressed;

	event.mouseButton.button = tgui::Event::MouseButton::Left;
	event.mouseButton.x = button->getPosition().x + button->getSize().x / 2;
	event.mouseButton.y = button->getPosition().y + button->getSize().y / 2;

	auto fX = (float)event.mouseButton.x;
	auto fY = (float)event.mouseButton.y;

	button->isMouseOnWidget({ fX, fY });
	button->leftMousePressed({ fX, fY });
	button->leftMouseReleased({ fX, fY });
}

void setupTestGui(flecs::world& world)
{
	world.import<canvas2d::module>();
	world.import<canvas2d::gui::module>();
	world.import<canvas2d::display::module>();
	world.entity().set<canvas2d::display::ScreenDims>({ 500, 500 , "Test window" });
	world.add<canvas2d::gui::GUI>();
}

TEST(GUITest, CanAddFunctionToButton)
{
	flecs::world world;

	setupTestGui(world);

	auto button = world.entity("TestButton")
		.add(canvas2d::gui::WidgetType::Button)
		.set<canvas2d::gui::ID>({ "test-id" });

	auto val = 0;
	auto func = ([&val] {val += 1; });

	canvas2d::gui::set_command(button, func);

	auto widget = world.get<canvas2d::gui::GUI>()->gui->get("test-id");
	auto castedWidget = std::dynamic_pointer_cast<tgui::Button>(widget);

	ASSERT_TRUE(castedWidget != nullptr) << "Widget is not a button.";
	
	simulateClick(castedWidget);
	ASSERT_EQ(1, val) << "Click event did not fire.";
}

TEST(GUITest, CannotAddFunctionToNonButton)
{
	flecs::world world;
	setupTestGui(world);

	auto label = world.entity("TestLabel")
		.add(canvas2d::gui::WidgetType::Label)
		.set<canvas2d::gui::ID>({ "test-id" });

	auto val = 0;
	auto func = ([&val] {val += 1; });

	ASSERT_THROW({
		canvas2d::gui::set_command(label, func);
		}, std::runtime_error);
}
