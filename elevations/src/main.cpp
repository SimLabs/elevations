#include "ui/main_window.h"
#include "dem/elevation_cursor.h"

using namespace elevations;

int main(int argc, char *argv[])
{
	QApplication application(argc, argv);
	ui::main_window window;
	window.show();
	auto widget = window.get_gl_widget();

	auto container = resource::resource_container::load_resource_container("Resources", "Resources/terrainDemo.xml");	
	widget->init(container.get());

	ptr<dem::elevation_cursor> cursor = new dem::elevation_cursor(container->get_cube_mapper());
	cursor->set_position(math::lat_lon_d(0, 0));
	cursor->leave_request(4);
	cursor->leave_request(7);

	return application.exec();
}
