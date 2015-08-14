#include "producer/height_layer.h"

#include <ork/resource/ResourceTemplate.h>
#include <ork/math/mat4.h>

namespace elevations
{
	namespace producer
	{
		class height_layer_resource : public ResourceTemplate<110, height_layer>
		{
		public:
			height_layer_resource(const ptr<ResourceManager>& resource_manager, const std::string& name, const ptr<ResourceDescriptor>& descriptor, const TiXmlElement* element = nullptr)
				: ResourceTemplate<110, height_layer>(resource_manager, name, descriptor)
			{
				element = element == nullptr ? descriptor->descriptor : element;
				checkParameters(descriptor, element, "name,cpuElevations,rotatex,rotatey,rotatez,");

				auto rotation_matrix = mat4d::IDENTITY;
				if (element->Attribute("rotatex") != nullptr)
				{
					float angle;
					getFloatParameter(descriptor, element, "rotatex", &angle);
					rotation_matrix = rotation_matrix * mat4d::rotatex(angle);
				}
				if (element->Attribute("rotatey") != nullptr)
				{
					float angle;
					getFloatParameter(descriptor, element, "rotatey", &angle);
					rotation_matrix = rotation_matrix * mat4d::rotatey(angle);
				}
				if (element->Attribute("rotatez") != nullptr)
				{
					float angle;
					getFloatParameter(descriptor, element, "rotatez", &angle);
					rotation_matrix = rotation_matrix * mat4d::rotatez(angle);
				}
				rotation_matrix = rotation_matrix.inverse();

				rotation_matrix_ = { rotation_matrix[0][0], rotation_matrix[0][1], rotation_matrix[0][2],
					rotation_matrix[1][0], rotation_matrix[1][1], rotation_matrix[1][2],
					rotation_matrix[2][0], rotation_matrix[2][1], rotation_matrix[2][2] };
				elevation_producer_ = manager->loadResource(getParameter(descriptor, element, "cpuElevations")).cast<proland::CPUElevationProducer>();
			}
		};

		extern const char HEIGHT_LAYER_STRING[] = "heightLayer";

		static ResourceFactory::Type<HEIGHT_LAYER_STRING, height_layer_resource> height_layer_type;
	} // producers
} // project namespace

using elevations::producer::height_layer;

height_layer::height_layer()
	: TileLayer(HEIGHT_LAYER_STRING, true)
{
	init(true);
}

size_t height_layer::get_min_level() const
{
	return elevation_producer_->getMinLevel();
}

size_t height_layer::get_max_level() const
{
	return elevation_producer_->getMaxLevel();
}

double height_layer::get_height(size_t level, double x, double y) const
{
	return proland::CPUElevationProducer::getHeight(elevation_producer_, level, x, y);
}

proland::TileCache::Tile* height_layer::get_tile(size_t level, int tx, int ty, unsigned deadline) const
{
	return elevation_producer_->getTile(level, tx, ty, deadline);
}

proland::TileCache::Tile* height_layer::find_tile(size_t level, int tx, int ty) const
{
	return elevation_producer_->findTile(level, tx, ty, true);
}

void height_layer::put_tile(proland::TileCache::Tile* tile) const
{
	elevation_producer_->putTile(tile);
}

vec3d height_layer::rotate(const vec3d& vector) const
{
	return rotation_matrix_ * vector;
}

void height_layer::schedule(ptr<TaskGraph> task_graph) const
{
	elevation_producer_->getCache()->getScheduler()->schedule(task_graph);
}

void height_layer::setTileSize(int tileSize, int tileBorder, float rootQuadSize)
{
	TileLayer::setTileSize(tileSize, tileBorder, rootQuadSize);
	elevation_producer_->setRootQuadSize(rootQuadSize);
}

bool height_layer::doCreateTile(int level, int tx, int ty, proland::TileStorage::Slot* data)
{
	return true;
}

void height_layer::swap(ptr<height_layer> other)
{
	TileLayer::swap(other);
	std::swap(rotation_matrix_, other->rotation_matrix_);
	std::swap(elevation_producer_, other->elevation_producer_);
}