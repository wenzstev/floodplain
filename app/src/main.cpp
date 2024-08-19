#include <iostream>
#include <flecs.h>
#include <direct.h>
#include <thread>
#include <random>
#include <filesystem>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <SFML/Graphics.hpp>
#include "grid.h"
#include "canvas2d.h"
#include "gui.h"
#include "display.h"
#include "input_processing.h"
#include "rendering.h"
#include "transform.h"
#include "agents.h"
#include "maputils.h"


int main(int, char* []) {

	flecs::world world;

	world.import<transform>();
	world.import<canvas2d::module>();
	world.import<grid_module>();
	world.import<agents>();
	world.import<flecs::monitor>();
	world.import<maputils::cameracontrols>();

	flecs::entity screenDims = world.entity()
		.set<canvas2d::display::ScreenDims>({ 1400, 1000, "Window in flecs" });

	auto gridPrefab = world.prefab("Rect")
		.set<canvas2d::rendering::Rectangle>({ 30, 30 })
		.add<canvas2d::rendering::DrawnIn, canvas2d::rendering::Background>()
		.set<agents::CarryingCapacity>({ 20 });

	flecs::entity g = world.entity("Grid")
		.set<transform::Position2, transform::World>({ 700, 500 })
		.set<grid_module::Grid>({
			gridPrefab,
			{30, 32},
			{30, 32}
			});

	flecs::entity agentPrefab = world.prefab("AgentPrefab")
		.set<agents::Age>({ 0 });

	world.defer_begin();
	auto f = world.filter<grid_module::Cell>();
	f.iter([](flecs::iter& it, grid_module::Cell* c)
		{
			for (auto i : it)
			{
				it.entity(i).set<transform::Color>({ 250, 250, 250, 255 });
			}
		});
	world.defer_end();

	world.add<canvas2d::gui::GUI>();

	auto camera = world.entity();
	camera.set<transform::Position2, canvas2d::display::ViewPos>({ 350, 300 });
	camera.set<transform::Position2, canvas2d::display::ViewScale>({ 300, 200 });

	struct DisplaysColor {};
	struct DisplaysPop {};

	world.component<DisplaysColor>().add(flecs::Exclusive);
	world.component<DisplaysPop>().add(flecs::Exclusive);

	world.system<canvas2d::gui::Text>()
		.with<DisplaysColor>(flecs::Wildcard)
		.each([](flecs::iter& it, size_t i, canvas2d::gui::Text& text)
			{
				auto displayCell = it.entity(i).target<DisplaysColor>();
				auto color = displayCell.get<transform::Color>();
				if (!color) return;
				it.entity(i).set<canvas2d::gui::Text>({ "Avg color: " + std::to_string(color->r) + ", " + std::to_string(color->g) + ", " + std::to_string(color->b) }); 
			});

	world.system<canvas2d::gui::Text>()
		.with<DisplaysPop>(flecs::Wildcard)
		.each([](flecs::iter& it, size_t i, canvas2d::gui::Text& text)
			{
				auto displayCell = it.entity(i).target<DisplaysPop>();
				int agentCount = 0;
				displayCell.children([&agentCount](flecs::entity child)
					{
						auto agent = child.get<agents::Agent>();
						if (!agent) return;
						agentCount++;
					});
				it.entity(i).set<canvas2d::gui::Text>({ "Agent count: " + std::to_string(agentCount) }); // works

			});

	world.system<canvas2d::input_processing::InputState, canvas2d::display::Screen, canvas2d::rendering::Rectangle, transform::Position2>("Display agents when square is clicked")
		.term_at(1).singleton()
		.term_at(2).singleton()
		.term_at(4).second<transform::World>()
		.each([](flecs::iter& it, size_t i, canvas2d::input_processing::InputState& inputState, canvas2d::display::Screen& camera, canvas2d::rendering::Rectangle& rect, transform::Position2& pos)
			{
				if (!inputState.MouseButtonPressed[sf::Mouse::Left]) return;

				auto mousePos = inputState.CursorLocation;
				sf::Vector2i point = { mousePos.x, mousePos.y };
				auto mouseWorldPos = camera.canvas->mapPixelToCoords(point);
				if (canvas2d::input_processing::is_inbounds(pos, rect, mouseWorldPos))
				{
					auto e = it.entity(i);
					auto PopulationLabel = it.world().lookup("LeftHandPanel::StatsPanel::PopulationLabel");
					PopulationLabel.add<DisplaysPop>(e);
					auto AvgColorLabel = it.world().lookup("LeftHandPanel::StatsPanel::AvgColorLabel");
					AvgColorLabel.add<DisplaysColor>(e);
				}
			});


	ecs_plecs_from_file(world, "gui.flecs");


	struct Ticker {
		int numTicks;
	};

	world.set<Ticker>({ 0 });

	auto TimeSinceStart = world.system<Ticker>("TimeSinceStart")
		.term_at(1).singleton()
		.each([](flecs::iter& it, size_t i, Ticker& ticker)
			{
				ticker.numTicks += 1;
			});

	struct DisplayTicks {};

	world.defer_begin();
	auto globalTickerEnt = world.lookup("LeftHandPanel::StatsPanel::TotalTicksLabel");
	globalTickerEnt.add<DisplayTicks>();
	world.defer_end();

	world.system<Ticker, canvas2d::gui::Text>()
		.term_at(1).singleton()
		.with<DisplayTicks>()
		.each([](flecs::entity e, Ticker& ticker, canvas2d::gui::Text& text)
			{
				e.set<canvas2d::gui::Text>({ "Time since start: " + std::to_string(ticker.numTicks) });
			});

	struct DisplayPop {};


	world.observer<agents::TotalPop>()
		.term_at(1).singleton()
		.event<agents::PopGained>()
		.each([](flecs::iter& it, size_t i, agents::TotalPop& totalPop)
			{
				totalPop.value += 1;
			});

	world.observer<agents::TotalPop>()
		.term_at(1).singleton()
		.event<agents::PopLost>()
		.each([](flecs::iter& it, size_t i, agents::TotalPop& totalPop)
			{
				totalPop.value -= 1;
			});


	world.defer_begin();
	auto globalPopEnt = world.lookup("LeftHandPanel::StatsPanel::GlobalPopulationLabel");
	globalPopEnt.add<DisplayPop>();
	world.defer_end();

	world.system<agents::TotalPop, canvas2d::gui::Text>()
		.term_at(1).singleton()
		.with<DisplayPop>()
		.each([](flecs::entity e, agents::TotalPop& totalPop, canvas2d::gui::Text& text)
			{
				e.set<canvas2d::gui::Text>({ "Population: " + std::to_string(totalPop.value) });
			});


	auto playButton = world.lookup("LeftHandPanel::ButtonPanel::PlayButton");
	canvas2d::gui::set_command(playButton, [&world, &TimeSinceStart](){ 
			agents::startAgentSystems(world); 
			TimeSinceStart.remove(flecs::Disabled);
		});

	auto pauseButton = world.lookup("LeftHandPanel::ButtonPanel::PauseButton");
	canvas2d::gui::set_command(pauseButton, [&world, &TimeSinceStart]() { 
			agents::stopAgentSystems(world); 
			TimeSinceStart.add(flecs::Disabled);
		});

	auto resetButton = world.lookup("LeftHandPanel::ButtonPanel::ResetButton");
	canvas2d::gui::set_command(resetButton, [&world]() {
			agents::clearAllAgents(world);
			agents::clearImpassableSquares(world);
			world.set<Ticker>({0});
		});


	auto myCallback = [&world](flecs::iter& it, size_t i, canvas2d::input_processing::InputState& input, canvas2d::display::Screen& screen)
		{
			if (!input.PressedKeys[sf::Keyboard::LShift] || !input.LastPressed == sf::Mouse::Left) return;
			auto clicked = canvas2d::input_processing::get_mouseover_rect(world, input, screen);
			if (clicked) agents::makeRandomAgent(it.world(), clicked);
		};

	canvas2d::input_processing::register_click_action(world, myCallback);

	auto toggleTraversable = [&world](flecs::iter& it, size_t i, canvas2d::input_processing::InputState& input, canvas2d::display::Screen& screen)
		{
			if (!input.LastPressed == sf::Mouse::Right) return;
			auto clicked = canvas2d::input_processing::get_mouseover_rect(world, input, screen);
			if (clicked) {
				if (clicked.has<agents::Impassable>()) 
					clicked.remove<agents::Impassable>();
				else clicked.add<agents::Impassable>();
			}
		};

	canvas2d::input_processing::register_click_action(world, toggleTraversable);

		
	world.set_threads(1);
	return world.app().enable_rest().run();
}
