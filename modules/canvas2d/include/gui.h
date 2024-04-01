#pragma once

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <flecs.h>

namespace canvas2d
{
	namespace gui {
		
		struct GUI
		{
			std::unique_ptr<tgui::Gui> gui;
		};

		struct Button
		{
			tgui::String title;
		};

		struct module {
			module(flecs::world& world);

		private:
			static void setup_gui(flecs::entity ent, GUI& gui);
			static void setup_button(flecs::entity ent, Button& button);
		};


	}
}