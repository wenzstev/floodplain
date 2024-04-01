#include "gui.h"
#include "display.h"

using namespace canvas2d;

gui::module::module(flecs::world& world)
{
	world.component<gui::GUI>()
		.on_add(setup_gui)
		.member<std::unique_ptr<tgui::Gui>>("gui");

	world.component<gui::Button>()
		.on_set(setup_button)
		.member<tgui::String>("Title");
}

void gui::module::setup_gui(flecs::entity ent, gui::GUI& gui)
{
	auto world = ent.world();
	auto screenEnt = world.get <display::Screen>();
	if (!screenEnt)
	{
		std::cout << "Need to create a screen before setting up the gui! \n";
		return;
	}

	auto foregroundPhase = world.lookup("canvas2d::rendering::module::ForegroundPhase");

	if (!foregroundPhase)
	{
		std::cout << "Error! Need to initialize rendering first. \n";
		return;
	}
	std::unique_ptr<tgui::Gui> guiPtr(new tgui::Gui(*screenEnt->canvas));
	world.set<gui::GUI>({ std::move(guiPtr) });


	world.system<GUI>("DrawGui")
		.kind(flecs::PreStore)
		.each([](flecs::iter& it, size_t i, GUI& gui)
			{
				gui.gui->draw();
			});

	std::cout << "Created gui!";
}

void gui::module::setup_button(flecs::entity ent, gui::Button& button)
{
	auto world = ent.world();
	auto gui = world.get<gui::GUI>();
	if (!gui)
	{
		std::cout << "Need to create a gui before adding a button! \n";
		return;
	}

	tgui::Button::Ptr newButton = tgui::Button::create(button.title);
	gui->gui->add(newButton, button.title);
	std::cout << "Created button!";


}