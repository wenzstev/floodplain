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
		.member<std::unique_ptr<sf::RenderWindow>>("canvas");

	world.component<View>()
		.on_add(init_view)
		.member<std::unique_ptr<sf::View>>("View");

	world.component<ViewPos>();
	world.component<ViewScale>();

	world.component<Rectangle>()
		.member<float>("width")
		.member<float>("height");

	world.component<SFMLEvent>()
		.member<sf::Event>("event");

	world.component<Rotation>()
		.member<float, flecs::units::angle>("angle");

	world.system<View, transform::Position2, transform::Position2>("MoveView")
		.term_at(2).first<ViewPos>()
		.term_at(3).first<ViewScale>()
		.each([](flecs::entity e, View& v, transform::Position2 pos, transform::Position2 scale)
			{
				v.v->setCenter(sf::Vector2(pos.x, pos.y));
				v.v->setSize(sf::Vector2(scale.x, scale.y));
			});
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
					const transform::Color* color = e.get<transform::Color>();
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

void canvas2d::init_view(flecs::entity viewEnt, View& viewComp)
{
	const transform::Position2* position = viewEnt.get<ViewPos, transform::Position2>();
	const transform::Position2* scale = viewEnt.get<ViewScale, transform::Position2>();

	if (!position || !scale)
	{
		std::cout << "Tried to add a view without position or scale components -- not allowed.";
		return;
	}

	std::unique_ptr<sf::View> viewPtr(new sf::View(
		sf::Vector2f(position->x, position->y), 
		sf::Vector2f(scale->x, scale->y)));

	viewComp.v = std::move(viewPtr);
	auto world = viewEnt.world();

	auto screen = world.get<canvas2d::Screen>();
	if (!screen)
	{
		std::cout << "Screen is null!" << "\n";
		return;

	}

	screen->canvas->setView(*viewComp.v);
}

void canvas2d::setup_canvas(flecs::world& world, ScreenDims& screenConfig)
{
	std::unique_ptr<sf::RenderWindow> window(new sf::RenderWindow(sf::VideoMode(screenConfig.xPixels, screenConfig.yPixels), screenConfig.screenName));
	world.set<Screen>({ std::move(window) });
	world.add<View>();

	world.system<canvas2d::Screen>()
		.each([](flecs::entity e, canvas2d::Screen& s)
			{
				sf::Event event;
				bool hasEvent = s.canvas->pollEvent(event);
				if (!hasEvent) return;
				auto eventEnt = e.world().entity("EventEntity").set<SFMLEvent>({event});
				e.world().event<sf::Event>().entity(eventEnt).emit();
			});

	world.observer<SFMLEvent, Screen>()
		.term_at(2).singleton()
		.with(flecs::Any)
		.event<sf::Event>()
		.each([](flecs::iter& it, size_t i, SFMLEvent& sfml_evt, Screen& screen)
			{
				auto e = it.entity(i);
				std::cout << e.name() << "\n";
				if (sfml_evt.event.type == sf::Event::Closed)
				{
					screen.canvas->close();
				}
				
			});

	world.observer<SFMLEvent, View>()
		.term_at(2).singleton()
		.with(flecs::Any)
		.event<sf::Event>()
		.each([](flecs::iter& it, size_t i, SFMLEvent& sfml_evt, View& v)
			{
				if (sfml_evt.event.type == sf::Event::KeyPressed)
				{
					auto k = sfml_evt.event.key;
					std::cout << "Key pressed: " << k.code << " \n";
				}
			});

	world.system<canvas2d::Screen>()
		.kind(flecs::OnStore)
		.each([](flecs::entity e, canvas2d::Screen& s)
			{
				s.canvas->display();
			});

	// using observer because I'm not sure how to do hooks with two components
	world.observer<transform::Position2, transform::Position2, canvas2d::Screen, canvas2d::View>()
		.term_at(1).second<ViewPos>()
		.term_at(2).second<ViewScale>()
		.term_at(3).singleton()
		.term_at(4).singleton()
		.event(flecs::OnSet)
		.each([](flecs::iter& it, size_t i, transform::Position2& pos, transform::Position2& scale, Screen& screen, View& view)
			{

				std::unique_ptr<sf::View> viewPtr(new sf::View(
					sf::Vector2f(pos.x, pos.y),
					sf::Vector2f(scale.x, scale.y)));

				view.v = std::move(viewPtr);
				screen.canvas->setView(*view.v);
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