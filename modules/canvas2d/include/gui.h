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

		struct Size {};

		enum WidgetType {
			Button,
			Label,
			Panel
		};

		struct CreatedWidget {};

		struct WidgetClicked {};


		/**
		 * @brief Set a command on a button.
		 *
		 * This function sets a command (callback) to be executed when the button is clicked.
		 *
		 * @param e The entity that contains the GUI and ID components.
		 * @param callback The function to be called when the button is clicked.
		 *
		 * @throws std::runtime_error if the GUI component is not found.
		 * @throws std::runtime_error if the ID component is not found.
		 * @throws std::runtime_error if the widget with the given ID is not registered.
		 * @throws std::runtime_error if the widget is not a button.
		 */
		void set_command(flecs::entity& e, std::function<void()> callback);

		struct module {
			module(flecs::world& world);

		private:
			static void setup_gui(flecs::entity ent, GUI& gui);
			static void setup_widget(flecs::entity ent, ID& id);
			
			static const GUI* get_gui(flecs::world& world);
			static std::variant <std::string, std::pair<std::string, std::string>> get_layout_info(flecs::entity ent);
			static void set_layout(std::shared_ptr<tgui::Widget> w, flecs::entity e);

			static std::shared_ptr<tgui::Widget> map_widget(const WidgetType& type);
		};


	}
}