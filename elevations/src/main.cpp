#include "ui/main_window.h"

using namespace elevations;

static static_ptr<resource::resource_container> RESOURCE_CONTAINER;

int main(int argc, char *argv[])
{
	QApplication application(argc, argv);
	ui::main_window window;
	window.show();
	auto widget = window.get_gl_widget();

	RESOURCE_CONTAINER = resource::resource_container::load_resource_container("Resources", "Resources/terrainDemo.xml");	
	widget->init(RESOURCE_CONTAINER.get());

	return application.exec();
}
