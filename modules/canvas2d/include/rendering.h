namespace canvas2d
{
	namespace rendering
	{
		struct Circle
		{
			float radius;
		};

		struct Rectangle
		{
			float width;
			float height;
		};

		struct DrawnIn {};
		struct Background {};
		struct Main {};
		struct Foreground {};

		struct module
		{
			module(flecs::world& world);
		private:
			static void setup_draw_phases(flecs::world& world);
		};
	}
}