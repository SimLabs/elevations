#include <ork/resource/XMLResourceLoader.h>
#include <ork/core/FileLogger.h>
#include <proland/TerrainPlugin.h>
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

int main(int argc, char *argv[])
{
	atexit(Object::exit);

	auto resource_manager = create_resource_manager("Resources", "Resources/terrainDemo.xml");
	auto lat_lon_converter = resource_manager->loadResource("latLonConverter").cast<dem::lat_lon_converter>();

	ptr<dem::elevation_cursor> cursor = new dem::elevation_cursor(lat_lon_converter.get());
	cursor->set_position(math::lat_lon_d(27.988056, 86.925278)); // Everest
	cursor->leave_request(7);
	cursor->leave_request(10);

	cursor->set_position(math::lat_lon_d(59.95, 30.316667)); // Saint-Petersburg
	cursor->leave_request(10);

	cursor->run();

	return 0;
}