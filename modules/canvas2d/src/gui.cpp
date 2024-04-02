#include "gui.h"
#include "display.h"
#include "transform.h"

using namespace canvas2d;

gui::module::module(flecs::world& world)
{
	world.component<gui::GUI>()
		.on_add(setup_gui)
		.member<std::unique_ptr<tgui::Gui>>("gui");

	world.component<gui::Button>()
		.on_set(setup_button);

	world.component<gui::Label>()
		.on_set(setup_label);
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
	auto gui = get_gui(world);

	auto text = ent.get<TString>();

	


	tgui::Button::Ptr newButton = tgui::Button::create(text ? text->text : "");
	gui->gui->add(newButton, button.id);
	std::cout << "Created button!";
}

void gui::module::setup_label(flecs::entity ent, gui::Label& label)
{
	auto world = ent.world();
	auto gui = get_gui(world);

	auto text = ent.get<TString>();

	tgui::Label::Ptr newLabel = tgui::Label::create(text ? text->text : "");
	gui->gui->add(newLabel, label.id);
	std::cout << "Created label!";
}

const gui::GUI* gui::module::get_gui(flecs::world& world)
{
	const gui::GUI* gui = world.get<gui::GUI>();
	if (!gui)
	{
		throw std::runtime_error("Need to create a gui before adding GUI components!");
	}
	return gui;
}

std::variant <const gui::TString*, std::pair<const gui::TString*, const gui::TString*>> gui::module::get_layout_info(flecs::entity ent)
{
	auto layout = ent.get_second<Layout, TString>();
	if (layout) return layout;
	
	

}