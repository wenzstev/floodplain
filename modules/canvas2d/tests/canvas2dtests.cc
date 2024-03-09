#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <SFML/Graphics.hpp>
#include "canvas2d.h"

class MockRenderWindow : public sf::RenderWindow {
public:
    MOCK_METHOD(void, clear, (const sf::Color&), ());
    MOCK_METHOD(void, draw, (const sf::Drawable&), ());
    MOCK_METHOD(void, display, (), ());
    MOCK_METHOD(bool, pollEvent, (sf::Event&), ());
    MOCK_METHOD(void, close, (), (override));
};

class MockScreen {
public:
    std::shared_ptr<MockRenderWindow> canvas = std::make_shared<MockRenderWindow>();
    // Add other members if `canvas2d::Screen` has any.
};

TEST(RenderSystemTest, RendersCorrectly)
{
    MockRenderWindow mockWindow;
    flecs::world world;

    world.import<canvas2d>();

    flecs::entity e = world.entity().set<canvas2d::Circle>({ 50 });

    flecs::entity screenDims = world.entity().set<canvas2d::ScreenDims>({ 800, 600, "Test window" });


    testing::Sequence seq;



    EXPECT_CALL(mockWindow, clear(sf::Color::Black))
        .InSequence(seq);
    EXPECT_CALL(mockWindow, draw(::testing::_))
        .InSequence(seq);
    EXPECT_CALL(mockWindow, display())
        .InSequence(seq);
    
    world.progress();


}