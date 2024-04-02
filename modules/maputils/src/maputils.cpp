#include "maputils.h"
#include "display.h"
#include "input_processing.h"


maputils::cameracontrols::cameracontrols(flecs::world& world)
{
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
}

