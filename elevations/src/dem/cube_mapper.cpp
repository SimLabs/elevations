#include "dem/cube_mapper.h"

#include <ork/resource/ResourceTemplate.h>

namespace elevations
{
	namespace dem
	{
		class cube_mapper_resource : public ResourceTemplate<120, cube_mapper>
		{
		public:
			cube_mapper_resource(const ptr<ResourceManager>& resource_manager, const std::string& name, const ptr<ResourceDescriptor>& descriptor, const TiXmlElement* element = nullptr)
				: ResourceTemplate<120, cube_mapper>(resource_manager, name, descriptor)
			{
				element = element == nullptr ? descriptor->descriptor : element;
				checkParameters(descriptor, element, "name,face1,face2,face3,face4,face5,face6,");

				for (size_t i = 0; i < FACES_NUMBER; ++i)
				{
					auto key = "face" + std::to_string(i + 1);
					elevation_producers_[i] = manager->loadResource(getParameter(descriptor, element, key.c_str())).cast<proland::CPUElevationProducer>();
				}
			}
		};

		extern const char ELEVATION_PRODUCER_CONTAINER_STRING[] = "cubeMapper";

		static ResourceFactory::Type<ELEVATION_PRODUCER_CONTAINER_STRING, cube_mapper_resource> height_layer_type;
	} // digital elevation model
} // project namespace

using elevations::dem::cube_mapper;

cube_mapper::cube_mapper()
	: Object(ELEVATION_PRODUCER_CONTAINER_STRING)
{
}

ptr<proland::CPUElevationProducer> cube_mapper::operator[](size_t index)
{
	assert(index < elevation_producers_.size());
	return elevation_producers_[index];
}

void cube_mapper::swap(ptr<cube_mapper> other)
{
	std::swap(elevation_producers_, other->elevation_producers_);
}