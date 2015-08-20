#include <queries/ui/view_manager.h>

#include <ork/core/FileLogger.h>
#include <ork/resource/ResourceTemplate.h>
#include <proland/util/PlanetViewController.h>

namespace elevations
{
	namespace ui
	{
		class view_manager_resource : public ResourceTemplate<100, view_manager>
		{
		public:
			view_manager_resource(ptr<ResourceManager> resource_manager, const std::string& name, ptr<ResourceDescriptor> descriptor, const TiXmlElement* element = nullptr)
				: ResourceTemplate<100, view_manager>(resource_manager, name, descriptor)
			{
				element = element == nullptr ? descriptor->descriptor : element;
				checkParameters(descriptor, element, "name,view,radius,dr,");

				scene_manager_ = new SceneManager();
				scene_manager_->setResourceManager(resource_manager);
				scene_manager_->setScheduler(resource_manager->loadResource("defaultScheduler").cast<Scheduler>());
				scene_manager_->setRoot(resource_manager->loadResource("scene").cast<SceneNode>());
				scene_manager_->setCameraNode("camera");
				scene_manager_->setCameraMethod("draw");

				if (element->Attribute("radius") != nullptr)
				{
					getFloatParameter(descriptor, element, "radius", &radius_);
				}
				if (element->Attribute("dr") != nullptr)
				{
					getFloatParameter(descriptor, element, "dr", &dr_);
				}
				view_handler_ = manager->loadResource(getParameter(descriptor, element, "view")).cast<proland::BasicViewHandler>();
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

		extern const char VIEW_MANAGER_STRING[] = "viewManager";

		static ResourceFactory::Type<VIEW_MANAGER_STRING, view_manager_resource> view_manager_type;

	} // user interface
} // project namespace

using elevations::ui::view_manager;

view_manager::view_manager(float radius, float dr)
	: Object(VIEW_MANAGER_STRING)
	, radius_(radius)
	, dr_(dr)
{
}

void view_manager::update_resources() const
{
	proland::BasicViewHandler::Position position;
	view_handler_->getPosition(position);
	scene_manager_->getResourceManager()->updateResources();
	view_controller_->setNode(scene_manager_->getCameraNode());
	view_handler_->setPosition(position);
}

ptr<proland::BasicViewHandler> view_manager::get_view_handler() const
{
	return view_handler_;
}

ptr<SceneManager> view_manager::getScene()
{
	return scene_manager_;
}

ptr<proland::TerrainViewController> view_manager::getViewController()
{
	if (view_controller_ == nullptr)
	{
		assert(radius_ > 0);
		auto camera_node = scene_manager_->getCameraNode();
		view_controller_ = new proland::PlanetViewController(camera_node, radius_);
	}
	return view_controller_;
}

vec3d view_manager::getWorldCoordinates(int x, int y)
{
	auto result = scene_manager_->getWorldCoordinates(x, y);
	auto view_controller = view_controller_.cast<proland::PlanetViewController>();
	if (view_controller != nullptr && result.length() > view_controller->R * dr_ || details::is_infinity(result))
	{
		result = vec3d(NAN, NAN, NAN);
	}
	return result;
}

void view_manager::swap(ptr<view_manager> other)
{
	using std::swap;
	swap(radius_, other->radius_);
	swap(dr_, other->dr_);
	swap(scene_manager_, other->scene_manager_);
	swap(view_handler_, other->view_handler_);
	swap(view_controller_, other->view_controller_);
}

const double view_manager::DISTANCE_INFINITY = 100000.0;

bool elevations::ui::details::is_infinity(double value)
{
	return value > view_manager::DISTANCE_INFINITY;
}

bool elevations::ui::details::is_infinity(const vec3<double>& vector)
{
	return is_infinity(vector.x) || is_infinity(vector.y) || is_infinity(vector.z);
}