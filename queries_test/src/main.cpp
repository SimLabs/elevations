#include <ork/resource/XMLResourceLoader.h>
#include <ork/core/FileLogger.h>
#include <proland/TerrainPlugin.h>
#include <queries/dem/elevation_cursor.h>
#include "counter.h"

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

	size_t n = 100;
	double lat_min = 20;
	double lat_max = 30;
	double lon_min = 80;
	double lon_max = 90;
	
	auto f = [&n](double min, double max, size_t k)
	{
		return min + (max - min) / n * k;
	};
	auto get_position = [&n, &lat_min, &lat_max, &lon_min, &lon_max, &f](size_t i, size_t j)
	{
		return math::lat_lon_d(f(lat_min, lat_max, i), f(lon_min, lon_max, j));
	};
	auto print = [](const math::lat_lon_d& lat_lon, ptr<dem::elevation_cursor> elevation_cursor)
	{
		std::cout << "position: " << lat_lon << ", " << elevation_cursor->get_current_height() << ", level: " << elevation_cursor->get_current_level() << std::endl;
	};
	
	counter counter;
	counter.start();
	for (size_t i = 0; i < n; ++i)
	{
		for (size_t j = 0; j < n; ++j)
		{
			ptr<dem::elevation_cursor> cursor = new dem::elevation_cursor(lat_lon_converter.get());
			auto position = get_position(i, j);
			cursor->set_position(position);
			cursor->leave_request(10);
			print(position, cursor);
		}
	}
	std::cout << counter.stop() << std::endl;
	std::cin.get();

	counter.start();
	ptr<dem::elevation_cursor> cursor = new dem::elevation_cursor(lat_lon_converter);
	for (size_t i = 0; i < n; ++i)
	{
		for (size_t j = 0; j < n; ++j)
		{
			auto position = get_position(i, j);
			cursor->set_position(position);
			cursor->leave_request(10);
			print(position, cursor);
		}
	}
	std::cout << counter.stop() << std::endl;
	std::cin.get();

	return 0;
}