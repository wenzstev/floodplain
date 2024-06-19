#include <variant>
#include <functional>
#include "gui.h"
#include "display.h"
#include "transform.h"
#include <iostream>

using namespace canvas2d;

template<typename WidgetType>
struct SupportsTextUpdate {
	static constexpr bool value = false;
};

template<>
struct SupportsTextUpdate<tgui::Button> {
	static constexpr bool value = true;
};

template<>
struct SupportsTextUpdate<tgui::Label> {
	static constexpr bool value = true;
};

template<typename WidgetType>
struct IsClickable {
	static constexpr bool value = false;
};

template<>
struct IsClickable<tgui::Button> {
	static constexpr bool value = true;
};



template<gui::WidgetType W, typename WidgetT>
void SetupWidgetObserver(
	flecs::world& world,
	std::function<std::shared_ptr<WidgetT>()> constructorFunction
) {
	world.observer<gui::GUI, gui::ID>()
		.with(W)
		.term_at(1).singleton()
		.event(flecs::OnSet)
		.each([constructorFunction](flecs::entity e, gui::GUI& gui, gui::ID& id)
			{
				auto widget = constructorFunction();
				if (widget) gui.gui->add(widget, id.id);
				e.add<gui::CreatedWidget>();

//				if constexpr (IsClickable<WidgetT>::value)
//				{
//					if (auto castedWidget = std::dynamic_pointer_cast<WidgetT>(widget))
//					{
//						auto ptr = std::make_shared<flecs::entity>(e);
//						castedWidget->onClick([ptr]() {
//							ptr->emit<gui::WidgetClicked>();
//							});
//					}
//				}
			});


	if constexpr (SupportsTextUpdate<WidgetT>::value) {
		world.observer<gui::GUI, gui::ID, gui::Text>()
			.with(W)
			.term_at(1).singleton()
			.event(flecs::OnSet)
			.each([](flecs::entity e, gui::GUI& gui, gui::ID& id, gui::Text& text)
				{
					auto widget = gui.gui->get(id.id);
					if (!widget) return;
					if (auto castedWidget = std::dynamic_pointer_cast<WidgetT>(widget))
					{
						castedWidget->setText(text.text);
					}
				});
	}

}



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
		.member<std::string>("ID");

	world.component<gui::LayoutX>()
		.member<std::string>("X Layout");

	world.component<gui::LayoutY>()
		.member<std::string>("Y Layout");

	world.component<gui::Layout>()
		.member<std::string>("Layout");

	world.component<gui::WidgetType>()
		.constant("Button", Button)
		.constant("Label", Label);

	world.component<Size>();


	SetupWidgetObserver<gui::WidgetType::Button, tgui::Button>(
		world,
		[]() -> std::shared_ptr<tgui::Button> { return tgui::Button::create(); }
	);

	SetupWidgetObserver<gui::WidgetType::Label, tgui::Label>(
		world,
		[]() -> std::shared_ptr<tgui::Label> { return tgui::Label::create(); }
	);	

	SetupWidgetObserver<gui::WidgetType::Panel, tgui::Panel>(
		world,
		[]() -> std::shared_ptr<tgui::Panel> { return tgui::Panel::create(); }
	);

	world.observer<gui::GUI, gui::ID, gui::LayoutX, gui::LayoutY>()
		.term_at(1).singleton()
		.event(flecs::OnSet)
		.each([](flecs::entity e, gui::GUI& gui, gui::ID& id, gui::LayoutX& layoutX, gui::LayoutY& layoutY)
			{
				auto widget = gui.gui->get(id.id);
				if (!widget) return;
				widget->setPosition({ layoutX.text }, { layoutY.text });
			});

	world.observer<gui::GUI, gui::ID, gui::Layout>()
		.term_at(1).singleton()
		.event(flecs::OnSet)
		.each([](flecs::entity e, gui::GUI& gui, gui::ID& id, gui::Layout& layout)
			{
				auto widget = gui.gui->get(id.id);
				if (!widget) return;
				widget->setPosition({ layout.text });
			});

	world.observer<gui::GUI, gui::ID, transform::Position2>()
		.term_at(1).singleton()
		.term_at(3).second<gui::Size>()
		.event(flecs::OnSet)
		.each([](flecs::entity e, gui::GUI& gui, gui::ID& id, transform::Position2& pos) 
			{
				auto widget = gui.gui->get(id.id);
				if (!widget) return;
				widget->setSize(pos.x, pos.y);
			});
	/*
	world.observer<gui::GUI, gui::ID, transform::Color>()
		.term_at(1).singleton()
		.with<CreatedWidget>()
		.event(flecs::OnSet)
		.each([](flecs::entity e, gui::GUI& gui, gui::ID& id, transform::Color& color)
			{
				auto widget = gui.gui->get(id.id);
				if (!widget) return;
				// TODO: Set the color
			});
			*/

	world.observer<gui::GUI, gui::ID, gui::ID>()
		.term_at(1).singleton()
		.term_at(3).parent().cascade()
		.term<WidgetType>(WidgetType::Panel).parent().cascade()
		.with<CreatedWidget>()
		.term<CreatedWidget>().parent().cascade()
		.event(flecs::OnSet)
		.each([](flecs::iter& it, size_t i, gui::GUI& gui, gui::ID& idChild, gui::ID& idParent)
			{
				auto childWidget = gui.gui->get(idChild.id);
				auto parentWidget = gui.gui->get(idParent.id);
			
				if (auto castedParent = std::dynamic_pointer_cast<tgui::Panel>(parentWidget))
				{
					std::cout << "Making " + idParent.id + " a parent of " + idChild.id << "\n";
					childWidget->getParent()->remove(childWidget); // remove old parent, unsure if necessary//
					castedParent->add(childWidget);
				}
				else
				{
					std::cout << "cast not working" << "\n";
				}
			});
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


void canvas2d::gui::set_command(flecs::entity& e, std::function<void()> callback)
{
	flecs::world world = e.world();
	auto gui = world.get<canvas2d::gui::GUI>();
	if (!gui) {
		throw std::runtime_error("Need a GUI to set a command!");
	}
	auto id = e.get<canvas2d::gui::ID>();
	if (!id) {
		throw std::runtime_error("The entity needs an ID component!");
	}
	auto widget = gui->gui->get(id->id);
	if (!widget) {
		throw std::runtime_error("Entity's ID does not match a registered widget.");
	}
	auto castedWidget = std::dynamic_pointer_cast<tgui::Button>(widget);
	if (castedWidget == nullptr)
	{
		throw std::runtime_error("Widget is not a button!");
	}

	castedWidget->onClick(callback);

}