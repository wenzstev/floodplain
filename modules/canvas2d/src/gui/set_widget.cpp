#include "gui.h"

using namespace canvas2d::gui;

static std::shared_ptr<tgui::Widget> map_widget(const WidgetType& type)
{
	switch (type)
	{
	case Button:
		return tgui::Button::create();
	case Label:
		return tgui::Label::create();
	default:
		throw std::invalid_argument("Unsupported widget type");
	}
}
