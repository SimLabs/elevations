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
	ptr<elevation_cursor> cursor = new elevation_cursor(RESOURCE_CONTAINER->get_elevations_producer().get());
	widget->init(RESOURCE_CONTAINER.get(), cursor.get());

	cursor->set_position(6350000, -6350000);
	cursor->leave_request(4);
	cursor->leave_request(7);

	return application.exec();
}
