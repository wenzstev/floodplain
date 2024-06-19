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
		.set<agents::CarryingCapacity>({ 20});
	
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



	flecs::entity cellA = world.lookup("Grid::Cell 0 0");
	flecs::entity cellB = world.lookup("Grid::Cell 20 1");
	flecs::entity cellC = world.lookup("Grid::Cell 25 25");

	auto agentA = world.entity().is_a(agentPrefab).set<agents::Agent>({ {100, 200, 100, 255} });
	auto agentB = world.entity().is_a(agentPrefab).set<agents::Agent>({ {200, 100, 100, 255} });
	auto agentC = world.entity().is_a(agentPrefab).set<agents::Agent>({ {100, 100, 200, 255} });


	agentA.child_of(cellA);
	agentB.child_of(cellB);
	agentC.child_of(cellC);

	
	auto camera = world.entity();
	camera.set<transform::Position2, canvas2d::display::ViewPos>({ 350, 300 });
	camera.set<transform::Position2, canvas2d::display::ViewScale>({ 300, 200 });


	auto isClicked = ([](const transform::Position2& pos, const canvas2d::rendering::Rectangle& rect, const sf::Vector2f& clickPos) {
		return clickPos.x >= pos.x && clickPos.x <= (pos.x + rect.width) &&
			clickPos.y >= pos.y && clickPos.y <= (pos.y + rect.height);
		});

	
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
				it.entity(i).set<canvas2d::gui::Text>({ "Avg color: " + std::to_string(color->r) + ", " + std::to_string(color->g) + ", " + std::to_string(color->b) }); // works
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
		.each([isClicked](flecs::iter& it, size_t i, canvas2d::input_processing::InputState& inputState, canvas2d::display::Screen& camera, canvas2d::rendering::Rectangle& rect, transform::Position2& pos)
			{
				if (!inputState.MouseButtonPressed[sf::Mouse::Left]) return;

				auto mousePos = inputState.CursorLocation;
				sf::Vector2i point = { mousePos.x, mousePos.y };
				auto mouseWorldPos = camera.canvas->mapPixelToCoords(point);
				if (isClicked(pos, rect, mouseWorldPos))
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

	world.system<Ticker>()
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


	world.observer<canvas2d::gui::GUI, canvas2d::gui::ID>()
		.term_at(1).singleton()
		.event<canvas2d::gui::WidgetClicked>()
		.each([](flecs::entity e, canvas2d::gui::GUI& gui, canvas2d::gui::ID& id)
			{
				if (e.name() != "val") return;
				std::cout << "Test";
				std::cout << e.name();
			});

	auto button = world.lookup("LeftHandPanel::StatsPanel::TestButton");
	canvas2d::gui::set_command(button, ([] {std::cout << "test!!"; }));
	
	
	

	world.set_threads(1);
	return world.app().enable_rest().run();
}