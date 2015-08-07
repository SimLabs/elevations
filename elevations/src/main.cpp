#include "ui/main_window.h"

using namespace elevations;

static static_ptr<resource_container> RESOURCE_CONTAINER;

int main(int argc, char *argv[])
{
	QApplication application(argc, argv);
	ui::main_window window;
	window.show();

	RESOURCE_CONTAINER = resource_container::load_resource_container("Resources", "Resources/terrainDemo.xml");
	window.get_gl_widget()->set_resource_container(RESOURCE_CONTAINER.get());

	return application.exec();
}
