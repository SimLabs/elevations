#include "ui/main_window.h"

using namespace elevations::ui;

main_window::main_window(QWidget* parent)
	: QMainWindow(parent)
{
	ui_.setupUi(this);
}

gl_widget* main_window::get_gl_widget() const
{
	return ui_.glWidget;
}