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

	world.component<Rotation>()
		.member<float, flecs::units::angle>("angle");

	world.component<OriginatesAt>();
}


template<typename ShapeComponent, typename DrawPhase, typename ShapeCreator>
void drawShape(flecs::world& world, ShapeCreator createShape, flecs::entity Phase) {


	world.system<ShapeComponent, canvas2d::Screen, canvas2d::DrawnIn>()
		.kind(Phase)
		.term_at(2).singleton()
		.term_at(3).second<DrawPhase>()
		.iter([createShape](flecs::iter& it, ShapeComponent* shapeComp, canvas2d::Screen* screen, canvas2d::DrawnIn* d) 
			{
				for (auto i : it)
				{
					auto shape = createShape(shapeComp[i]);
					auto e = it.entity(i);
					const canvas2d::Color* color = e.get<canvas2d::Color>();
					if (color) {
						shape.setFillColor(sf::Color(color->r, color->g, color->b, color->a));
					}
					const transform::Position2* pos = e.get<transform::Position2, transform::World>();
					if (pos) {
						shape.setPosition(pos->x, pos->y);
					}
					const canvas2d::Rotation* rot = e.get<canvas2d::Rotation>();
					if (rot) {
						shape.setRotation(rot->angle);
					}
					screen -> canvas->draw(shape);
				}
			});
}


void canvas2d::init_window(flecs::entity screen, ScreenDims& screenConfig)
{
	flecs::world world = screen.world();
	setup_canvas(world, screenConfig);
	setup_draw_phases(world);
}

void canvas2d::setup_canvas(flecs::world& world, ScreenDims& screenConfig)
{
	std::unique_ptr<sf::RenderWindow> window(new sf::RenderWindow(sf::VideoMode(screenConfig.xPixels, screenConfig.yPixels), screenConfig.screenName));
	world.set<Screen>({ std::move(window) });

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

void canvas2d::setup_draw_phases(flecs::world& world)
{
	flecs::entity BackgroundPhase = world.entity("BackgroundPhase")
		.add(flecs::Phase)
		.depends_on(flecs::OnUpdate);

	flecs::entity MainDrawPhase = world.entity("MainDrawPhase")
		.add(flecs::Phase)
		.depends_on(BackgroundPhase);

	flecs::entity ForegroundPhase = world.entity("ForegroundPhase")
		.add(flecs::Phase)
		.depends_on(MainDrawPhase);

	world.system<canvas2d::Screen>()
		.kind(flecs::PreUpdate)
		.each([](flecs::entity e, canvas2d::Screen& s)
			{
				s.canvas->clear(sf::Color::Black);
			});

	auto drawCircle = [](const canvas2d::Circle& c) -> sf::CircleShape {
		sf::CircleShape circle(c.radius);
		return circle;
		};

	auto drawRect = [](const canvas2d::Rectangle& r) -> sf::RectangleShape {
		sf::RectangleShape rect(sf::Vector2(r.width, r.height));
		return rect;
		};

	drawShape<canvas2d::Circle, canvas2d::Background>(world, drawCircle, BackgroundPhase);
	drawShape<canvas2d::Circle, canvas2d::Main>(world, drawCircle, MainDrawPhase);
	drawShape<canvas2d::Circle, canvas2d::Foreground>(world, drawCircle, ForegroundPhase);

	drawShape<canvas2d::Rectangle, canvas2d::Background>(world, drawRect, BackgroundPhase);
	drawShape<canvas2d::Rectangle, canvas2d::Main>(world, drawRect, MainDrawPhase);
	drawShape<canvas2d::Rectangle, canvas2d::Foreground>(world, drawRect, ForegroundPhase);
}