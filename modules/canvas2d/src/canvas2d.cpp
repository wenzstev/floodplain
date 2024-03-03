#include "canvas2d.h"
#include <flecs.h>
#include <SFML/Graphics.hpp>

canvas2d::canvas2d(flecs::world& world)
{
	world.import<flecs::units>();

	world.component<Circle>()
		.member<float>("Radius");

	world.component<ScreenDims>()
		.on_set(init_window)
		.member<int>("width")
		.member<int>("height")
		.member<std::string>("name");

	world.component<Screen>()
		.member<std::shared_ptr<sf::RenderWindow>>("canvas");

	world.component<Rectangle>()
		.member<float>("width")
		.member<float>("height");

	world.component<Color>()
		.member<float>("Red")
		.member<float>("Green")
		.member<float>("Blue")
		.member<float>("Alpha");

	world.component<Vector2>()
		.member<float>("X Coordinate")
		.member<float>("Y Coordinate");

	world.component<Rotation>()
		.member<float, flecs::units::angle>("angle");

	world.component<OriginatesAt>();
}


template<typename ShapeComponent, typename ShapeCreator>
void drawShape(flecs::world& world, ShapeCreator createShape) {
	world.system<ShapeComponent, canvas2d::Screen>()
		.kind(flecs::OnUpdate)
		.term_at(2).singleton()
		.each([createShape](flecs::entity e, ShapeComponent& shapeComp, canvas2d::Screen& screen) 
			{
				auto shape = createShape(shapeComp);
				const canvas2d::Color* color = e.get<canvas2d::Color>();
				if (color) {
					shape.setFillColor(sf::Color(color->r, color->g, color->b, color->a));
				}
				const canvas2d::Vector2* pos = e.get<canvas2d::Vector2>();
				if (pos) {
					shape.setPosition(pos->x, pos->y);
				}
				const canvas2d::Rotation* rot = e.get<canvas2d::Rotation>();
				if (rot) {
					shape.setRotation(rot->angle);
				}
				screen.canvas->draw(shape);
			});
}


void canvas2d::init_window(flecs::entity screen, ScreenDims& screenConfig)
{
	std::unique_ptr<sf::RenderWindow> window (new sf::RenderWindow(sf::VideoMode(screenConfig.xPixels, screenConfig.yPixels), screenConfig.screenName));
	flecs::world world = screen.world();
	world.set<Screen>({ std::move(window) });

	world.system<canvas2d::Screen>()
		.kind(flecs::PreUpdate)
		.each([](flecs::entity e, canvas2d::Screen& s)
			{	
				s.canvas->clear(sf::Color::Black);
			});

	

	drawShape<canvas2d::Circle>(world, [](const canvas2d::Circle& c)-> sf::CircleShape {
		sf::CircleShape circle(c.radius);
		return circle;
		});

	drawShape<canvas2d::Rectangle>(world, [](const canvas2d::Rectangle& r) -> sf::RectangleShape {
		sf::RectangleShape rect(sf::Vector2(r.width, r.height));
		return rect;
		});


	world.system<canvas2d::Screen>()
		.each([](flecs::entity e, canvas2d::Screen& s)
			{
				sf::Event event;
				bool hasEvent = s.canvas->pollEvent(event);
				if (event.type == sf::Event::Closed) s.canvas->close();
			});

	world.system<canvas2d::Screen>()
		.kind(flecs::OnStore)
		.each([](flecs::entity e, canvas2d::Screen& s)
			{
				s.canvas->display();
			});


}
