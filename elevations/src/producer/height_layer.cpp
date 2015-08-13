#include "producer/height_layer.h"

#include <ork/resource/ResourceTemplate.h>

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
				checkParameters(descriptor, element, "name,cpuElevations,");

				elevation_producer_ = manager->loadResource(getParameter(descriptor, element, "cpuElevations")).cast<proland::CPUElevationProducer>();
			}
		};

		extern const char HEIGHT_LAYER_STRING[] = "heightLayer";

		static ResourceFactory::Type<HEIGHT_LAYER_STRING, height_layer_resource> height_layer_type;
	} // producers
} // project namespace

using elevations::producer::height_layer;

height_layer::height_layer(bool deform)
	: TileLayer(HEIGHT_LAYER_STRING, deform)
{
	init(deform);
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
	std::swap(elevation_producer_, other->elevation_producer_);
}