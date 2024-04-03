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

		struct TString
		{
			tgui::String text;
		};

		struct LayoutX {};
		struct LayoutY {};
		struct Layout {};

	

		struct Button 
		{
			tgui::String id;
		};
		struct Label 
		{
			tgui::String id;
		};

		struct module {
			module(flecs::world& world);

		private:
			static void setup_gui(flecs::entity ent, GUI& gui);
			static void setup_button(flecs::entity ent, Button& button);
			static void setup_label(flecs::entity ent, gui::Label& label);
			
			static const GUI* get_gui(flecs::world& world);
			static std::variant <std::string, std::pair<std::string, std::string>> get_layout_info(flecs::entity ent);
		};


	}
}