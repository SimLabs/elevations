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
					height_layers_[i] = manager->loadResource(getParameter(descriptor, element, key.c_str())).cast<producer::height_layer>();
				}
			}
		};

		extern const char ELEVATION_PRODUCER_CONTAINER_STRING[] = "cubeMapper";

		static ResourceFactory::Type<ELEVATION_PRODUCER_CONTAINER_STRING, cube_mapper_resource> height_layer_type;
	} // digital elevation model
} // project namespace

using elevations::dem::cube_mapper;

cube_mapper::cube_mapper(float radius)
	: Object(ELEVATION_PRODUCER_CONTAINER_STRING)
{
	init(radius);
}

void cube_mapper::init(float radius)
{
	assert(radius > 0);
	spherical_deformation_ = new proland::SphericalDeformation(radius);
}

ptr<elevations::dem::location> cube_mapper::to_location(const elevations::math::lat_lon_d& lat_lon) const
{
	auto cube_face_index = lat_lon.determine_cube_face();
	assert(cube_face_index != -1);

	auto height_layer = height_layers_[cube_face_index];
	auto vector = spherical_deformation_->deformedToLocal(height_layer->rotate(lat_lon.to_cartesian()));
	return new location(vector.x, vector.y, height_layer);
}

void cube_mapper::swap(ptr<cube_mapper> other)
{
	std::swap(spherical_deformation_, other->spherical_deformation_);
	std::swap(height_layers_, other->height_layers_);
}