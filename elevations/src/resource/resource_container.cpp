#include "resource/resource_container.h"

#include <ork/core/FileLogger.h>
#include <ork/resource/XMLResourceLoader.h>
#include <ork/resource/ResourceTemplate.h>
#include <proland/util/PlanetViewController.h>
#include <proland/TerrainPlugin.h>

namespace elevations
{
	namespace resource
	{
		class resource_container_resource : public ResourceTemplate<100, resource_container>
		{
		public:
			resource_container_resource(ptr<ResourceManager> resource_manager, const std::string& name, ptr<ResourceDescriptor> descriptor, const TiXmlElement* element = nullptr)
				: ResourceTemplate<100, resource_container>(resource_manager, name, descriptor)
			{
				element = element == nullptr ? descriptor->descriptor : element;
				checkParameters(descriptor, element, "name,view,radius,distance,dr,cpuElevations,");

				scene_manager_ = new SceneManager();
				scene_manager_->setResourceManager(resource_manager);
				scene_manager_->setScheduler(resource_manager->loadResource("defaultScheduler").cast<Scheduler>());
				scene_manager_->setRoot(resource_manager->loadResource("scene").cast<SceneNode>());
				scene_manager_->setCameraNode("camera");
				scene_manager_->setCameraMethod("draw");

				view_handler_ = manager->loadResource(getParameter(descriptor, element, "view")).cast<proland::BasicViewHandler>();
				elevation_producer_ = manager->loadResource(getParameter(descriptor, element, "cpuElevations")).cast<proland::CPUElevationProducer>();
				if (element->Attribute("radius") != nullptr)
				{
					getFloatParameter(descriptor, element, "radius", &radius_);
				}
				if (element->Attribute("distance") != nullptr)
				{
					getFloatParameter(descriptor, element, "distance", &distance_);
				}
				if (element->Attribute("dr") != nullptr)
				{
					getFloatParameter(descriptor, element, "dr", &dr_);
				}
			}

			virtual void doRelease() override
			{
				if (manager != nullptr)
				{
					manager->close();
				}
				delete this;
			}
		};

		extern const char RESOURCE_CONTAINER_STRING[] = "resourceContainer";

		static ResourceFactory::Type<RESOURCE_CONTAINER_STRING, resource_container_resource> resource_container_type;
	} // resource management
} // project namespace

using elevations::resource::resource_container;

resource_container::resource_container()
	: Object(RESOURCE_CONTAINER_STRING)
	, radius_(0.0f)
	, distance_(50000.0f)
	, dr_(1.1f)
{
}

void resource_container::update_resources() const
{
	proland::BasicViewHandler::Position position;
	view_handler_->getPosition(position);
	scene_manager_->getResourceManager()->updateResources();
	view_controller_->setNode(scene_manager_->getCameraNode());
	view_handler_->setPosition(position);
}

ptr<proland::BasicViewHandler> resource_container::get_view_handler() const
{
	return view_handler_;
}

ptr<proland::CPUElevationProducer> resource_container::get_elevations_producer() const
{
	return elevation_producer_;
}

ptr<SceneManager> resource_container::getScene()
{
	return scene_manager_;
}

ptr<proland::TerrainViewController> resource_container::getViewController()
{
	if (view_controller_ == nullptr)
	{
		auto camera_node = scene_manager_->getCameraNode();
		view_controller_ = radius_ == 0.0f ?
			new proland::TerrainViewController(camera_node, distance_) :
			new proland::PlanetViewController(camera_node, radius_);
	}
	return view_controller_;
}

vec3d resource_container::getWorldCoordinates(int x, int y)
{
	auto result = scene_manager_->getWorldCoordinates(x, y);
	auto view_controller = view_controller_.cast<proland::PlanetViewController>();
	if (view_controller != nullptr && result.length() > view_controller->R * dr_ || details::is_infinity(result))
	{
		result = vec3d(NAN, NAN, NAN);
	}
	return result;
}

ptr<resource_container> resource_container::load_resource_container(const std::string& path, const std::string& filename, const std::string& log_filename, size_t cache_size)
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

	ptr<ResourceManager> resource_manager = new ResourceManager(resource_loader, cache_size);
	return resource_manager->loadResource(RESOURCE_CONTAINER_STRING).cast<resource_container>();
}

void resource_container::swap(ptr<resource_container> other)
{
	using std::swap;
	swap(radius_, other->radius_);
	swap(distance_, other->distance_);
	swap(dr_, other->dr_);
	swap(scene_manager_, other->scene_manager_);
	swap(view_handler_, other->view_handler_);
	swap(view_controller_, other->view_controller_);
}

const double resource_container::DISTANCE_INFINITY = 100000.0;