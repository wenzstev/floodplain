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


int main(int, char* []) {

	flecs::world world;

	world.import<transform>();
	world.import<canvas2d::module>();
	world.import<grid_module>();
	world.import<agents>();
	world.import<flecs::monitor>();

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


	flecs::entity cellA = world.lookup("Grid::Cell 1 10");
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

	world.system<canvas2d::display::View, canvas2d::input_processing::InputState, canvas2d::display::Screen>("CameraMove")
		.term_at(1).singleton()
		.term_at(2).singleton()
		.each([](flecs::iter& it, size_t i, canvas2d::display::View& v, canvas2d::input_processing::InputState& inputState, canvas2d::display::Screen& screen)
			{
				float moveAmount = 10.f;
				sf::Vector2f moveVec {0, 0};
				if (inputState.PressedKeys[sf::Keyboard::W])
				{
					moveVec.y -= moveAmount;
				}
				if (inputState.PressedKeys[sf::Keyboard::A])
				{
					moveVec.x -= moveAmount;
				}
				if (inputState.PressedKeys[sf::Keyboard::S])
				{
					moveVec.y += moveAmount;
				}
				if (inputState.PressedKeys[sf::Keyboard::D])
				{
					moveVec.x += moveAmount;
				}
				v.v->move(moveVec);
				screen.canvas->setView(*v.v);
			});


	world.system<canvas2d::display::View, canvas2d::input_processing::InputState, canvas2d::display::Screen>("CameraZoom")
		.term_at(1).singleton()
		.term_at(2).singleton()
		.each([](flecs::iter& it, size_t i, canvas2d::display::View& v, canvas2d::input_processing::InputState& inputState, canvas2d::display::Screen& screen)
			{
				float zoomAmount = 2;
				if (inputState.PressedKeys[sf::Keyboard::Q])
				{
					v.v->zoom(1.2);
					screen.canvas->setView(*v.v);
				}
				if (inputState.PressedKeys[sf::Keyboard::E])
				{
					v.v->zoom(0.8);
					screen.canvas->setView(*v.v);
				}
			});

	auto isClicked = ([](const transform::Position2& pos, const canvas2d::rendering::Rectangle& rect, const sf::Vector2f& clickPos) {
		return clickPos.x >= pos.x && clickPos.x <= (pos.x + rect.width) &&
			clickPos.y >= pos.y && clickPos.y <= (pos.y + rect.height);
		});

	world.system<canvas2d::input_processing::InputState, canvas2d::display::Screen, canvas2d::rendering::Rectangle, transform::Position2>("Display agents when square is clicked")
		.term_at(1).singleton()
		.term_at(2).singleton()
		.term_at(4).second<transform::World>()
		.each([isClicked](flecs::iter& it, size_t i, canvas2d::input_processing::InputState& inputState, canvas2d::display::Screen& camera, canvas2d::rendering::Rectangle& rect, transform::Position2& pos)
			{

				// if click happened
				//		determine if click happened on a square
				//		get the children of that square
				//		cout the children

				if (!inputState.MouseButtonPressed[sf::Mouse::Left]) return;

				auto mousePos = inputState.CursorLocation;
				sf::Vector2i point = { mousePos.x, mousePos.y };
				auto mouseWorldPos = camera.canvas->mapPixelToCoords(point);
				if (isClicked(pos, rect, mouseWorldPos))
				{
					auto e = it.entity(i);
					e.children([](flecs::entity child)
						{
							auto agent = child.get<agents::Agent>();
							if (!agent) return;
							std::cout << child.name() << " " << agent->color.r << " " << agent->color.g << " " << agent->color.b << "\n";
						});
				}
			});

	world.add<canvas2d::gui::GUI>();

	auto buttonEnt = world.entity("Button")
		.set<canvas2d::gui::Button>({ "Test button" });

	

	world.set_threads(12);
	return world.app().enable_rest().run();

}