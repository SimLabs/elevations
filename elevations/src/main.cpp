#include "ui/main_window.h"

using namespace elevations;

static static_ptr<resource_container> RESOURCE_CONTAINER;

int main(int argc, char *argv[])
{
	QApplication application(argc, argv);
	ui::main_window window;
	window.show();
	auto widget = window.get_gl_widget();

	RESOURCE_CONTAINER = resource_container::load_resource_container("Resources", "Resources/terrainDemo.xml");
	elevation_cursor cursor(RESOURCE_CONTAINER->get_elevations_producer());
	widget->init(RESOURCE_CONTAINER.get(), &cursor);

	cursor.set_position(0, 3200000);

	return application.exec();
}
