#include <variant>
#include "gui.h"
#include "display.h"
#include "transform.h"

using namespace canvas2d;

gui::module::module(flecs::world& world)
{
	world.component<gui::GUI>()
		.on_add(setup_gui)
		.member<std::unique_ptr<tgui::Gui>>("gui");

	world.component<std::string>()
		.opaque(flecs::String)
		.serialize([](const flecs::serializer* s, const std::string* data) {
			const char* str = data->c_str();
			return s->value(flecs::String, &str);
			})
		.assign_string([](std::string* data, const char* value) {
				*data = value;
			});

	world.component<gui::Text>()
		.member<std::string>("text");


	world.component<gui::ID>()
		.on_set(setup_widget)
		.member<std::string>("ID");


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

void gui::module::setup_widget(flecs::entity ent, gui::ID& id)
{
	auto world = ent.world();
	auto gui = get_gui(world);

	auto text = ent.get<Text>();

	auto type = ent.get<gui::WidgetType>();
	std::shared_ptr<tgui::Widget> widget;
	if (!type) return;
	switch (type[0])
	{
	case Button:
		widget = tgui::Button::create(text ? text->text : "");
		break;
	case Label:
		widget = tgui::Label::create(text ? text->text : "");
		break;
	default:
		break;
	}

	set_layout(widget, ent);
	gui->gui->add(widget, id.id);
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

void gui::module::set_layout(std::shared_ptr<tgui::Widget> w, flecs::entity e)
{
	auto result = get_layout_info(e);
	std::visit([w](auto&& arg) {
		using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, std::string>) {
			w->setPosition({ arg });
		}
		else if constexpr (std::is_same_v<T, std::pair<std::string, std::string>>) {
			w->setPosition({ arg.first }, { arg.second });
		}
		}, result);
}

std::variant <std::string, std::pair<std::string, std::string>> gui::module::get_layout_info(flecs::entity ent)
{
	auto layout = ent.get<gui::Layout>();
	if (layout) return layout->text;

	auto layoutX = ent.get<LayoutX>();
	auto layoutY = ent.get<LayoutY>();

	return std::make_pair(layoutX ? layoutX->text : "", layoutY ? layoutY->text : "");
}