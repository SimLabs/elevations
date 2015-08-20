#include <ork/resource/XMLResourceLoader.h>
#include <ork/core/FileLogger.h>
#include <proland/TerrainPlugin.h>

#include "ui/main_window.h"
#include <queries/dem/elevation_cursor.h>

using namespace elevations;
using std::string;

ptr<ResourceManager> create_resource_manager(const string& path,
	const string& filename,
	const string& log_filename = "log.html",
	size_t cache_size = 8)
{
	proland::initTerrainPlugin();

	auto out = new FileLogger::File(log_filename);
	Logger::DEBUG_LOGGER = new FileLogger("DEBUG", out, Logger::DEBUG_LOGGER);
	Logger::INFO_LOGGER = new FileLogger("INFO", out, Logger::INFO_LOGGER);
	Logger::WARNING_LOGGER = new FileLogger("WARNING", out, Logger::WARNING_LOGGER);
	Logger::ERROR_LOGGER = new FileLogger("ERROR", out, Logger::ERROR_LOGGER);

	ptr<XMLResourceLoader> resource_loader = new XMLResourceLoader();
	resource_loader->addPath(path);
	resource_loader->addArchive(filename);

	return new ResourceManager(resource_loader, cache_size);
}

template <typename T>
ptr<T> load_resource(ptr<ResourceManager> resource_manager, const string& resource_name)
{
	return resource_manager->loadResource(resource_name).cast<T>();
}

static static_ptr<ui::view_manager> view_manager;

int main(int argc, char *argv[])
{
	QApplication application(argc, argv);
	ui::main_window window;
	window.show();
	auto widget = window.get_gl_widget();

	auto resource_manager = create_resource_manager("Resources", "Resources/terrainDemo.xml");
	view_manager = load_resource<ui::view_manager>(resource_manager, "viewManager");
	auto lat_lon_converter = load_resource<dem::lat_lon_converter>(resource_manager, "cubeMapper");

	widget->init(view_manager.get());

	auto cursor = new dem::elevation_cursor(lat_lon_converter.get());
	cursor->set_position(math::lat_lon_d(27.988056, 86.925278)); // Everest
	cursor->leave_request(7);
	cursor->leave_request(10);

	cursor->set_position(math::lat_lon_d(59.95, 30.316667)); // Saint-Petersburg
	cursor->leave_request(10);

	return application.exec();
}
