#include <flecs.h>
#include <SFML/Graphics.hpp>
#include "rendering.h"
#include "display.h"
#include "transform.h"

canvas2d::rendering::module::module(flecs::world& world)
{
	world.component<Circle>()
		.member<float>("Radius");

	world.component<Rectangle>()
		.member<float>("width")
		.member<float>("height");

	setup_draw_phases(world);
}

template<typename ShapeComponent, typename DrawPhase, typename ShapeCreator>
void drawShape(flecs::world& world, ShapeCreator createShape, flecs::entity Phase) {

	world.system<ShapeComponent, canvas2d::display::Screen, canvas2d::rendering::DrawnIn>()
		.kind(Phase)
		.term_at(2).singleton()
		.term_at(3).second<DrawPhase>()
		.iter([createShape](flecs::iter& it, ShapeComponent* shapeComp, canvas2d::display::Screen* screen, canvas2d::rendering::DrawnIn* d)
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
					const transform::Rotation* rot = e.get<transform::Rotation>();
					if (rot) {
						shape.setRotation(rot->angle);
					}
					screen->canvas->draw(shape);
				}
			});
}

void canvas2d::rendering::module::setup_draw_phases(flecs::world& world)
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

	auto drawCircle = [](const Circle& c) -> sf::CircleShape {
		sf::CircleShape circle(c.radius);
		return circle;
	};

	auto drawRect = [](const Rectangle& r) -> sf::RectangleShape {
		sf::RectangleShape rect(sf::Vector2(r.width, r.height));
		return rect;
	};

	drawShape<Circle, Background>(world, drawCircle, BackgroundPhase);
	drawShape<Circle, Main>(world, drawCircle, MainDrawPhase);
	drawShape<Circle, Foreground>(world, drawCircle, ForegroundPhase);

	drawShape<Rectangle, Background>(world, drawRect, BackgroundPhase);
	drawShape<Rectangle, Main>(world, drawRect, MainDrawPhase);
	drawShape<Rectangle, Foreground>(world, drawRect, ForegroundPhase);
}
