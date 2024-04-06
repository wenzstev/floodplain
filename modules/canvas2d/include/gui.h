#pragma once

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <flecs.h>
#include <variant>

namespace canvas2d
{
	namespace gui {

		struct GUI
		{
			std::unique_ptr<tgui::Gui> gui;
		};

		struct LayoutX {
			std::string text;
		};

		struct LayoutY {
			std::string text;
		};

		struct Layout {
			std::string text;
		};

		struct ID {
			std::string id;
		};

		struct Text
		{
			std::string text;
		};

		enum WidgetType {
			Button,
			Label
		};



		struct module {
			module(flecs::world& world);

		private:
			static void setup_gui(flecs::entity ent, GUI& gui);
			static void setup_widget(flecs::entity ent, ID& id);
			
			static const GUI* get_gui(flecs::world& world);
			static std::variant <std::string, std::pair<std::string, std::string>> get_layout_info(flecs::entity ent);
			static void set_layout(std::shared_ptr<tgui::Widget> w, flecs::entity e);
		};


	}
}