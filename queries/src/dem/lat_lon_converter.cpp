﻿#include <queries/dem/lat_lon_converter.h>

#include <ork/resource/ResourceTemplate.h>

namespace elevations
{
	namespace dem
	{
		class lat_lon_converter_resource : public ResourceTemplate<110, lat_lon_converter>
		{
		public:
			lat_lon_converter_resource(const ptr<ResourceManager>& resource_manager, const std::string& name, const ptr<ResourceDescriptor>& descriptor, const TiXmlElement* element = nullptr)
				: ResourceTemplate<110, lat_lon_converter>(resource_manager, name, descriptor)
			{
				element = element == nullptr ? descriptor->descriptor : element;
				checkParameters(descriptor, element, "name,radius,face1,face2,face3,face4,face5,face6,");
				
				for (size_t i = 0; i < FACES_NUMBER; ++i)
				{
					auto key = "face" + std::to_string(i + 1);
					elevation_producers_[i] = manager->loadResource(getParameter(descriptor, element, key.c_str())).cast<proland::CPUElevationProducer>();
				}
				if (element->Attribute("radius") != nullptr)
				{
					float radius;
					getFloatParameter(descriptor, element, "radius", &radius);
					init(radius);
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

		extern const char LAT_LON_CONVERTER_STRING[] = "latLonConverter";

		static ResourceFactory::Type<LAT_LON_CONVERTER_STRING, lat_lon_converter_resource> lat_lon_converter_type;
	} // digital elevation model
} // project namespace

using elevations::dem::lat_lon_converter;

lat_lon_converter::lat_lon_converter(float radius)
	: Object(LAT_LON_CONVERTER_STRING)
{
	init(radius);
}

void lat_lon_converter::init(float radius)
{
	assert(radius > 0);
	spherical_deformation_ = new proland::SphericalDeformation(radius);
	for (auto elevation_producer : elevation_producers_)
	{
		if (elevation_producer != nullptr)
		{
			elevation_producer->setRootQuadSize(2 * radius);
		}
	}
}

lat_lon_converter::~lat_lon_converter()
{
}

elevations::dem::location lat_lon_converter::to_location(const elevations::math::lat_lon_d& lat_lon) const
{
	auto cube_face_index = lat_lon.determine_cube_face();
	assert(cube_face_index != -1);

	auto elevation_producer = elevation_producers_[cube_face_index];
	auto vector = spherical_deformation_->deformedToLocal(elevation_producer->rotate(lat_lon.to_cartesian()));
	return location(vector.x, vector.y, elevation_producer.get());
}

void lat_lon_converter::swap(ptr<lat_lon_converter> other)
{
	std::swap(spherical_deformation_, other->spherical_deformation_);
	std::swap(elevation_producers_, other->elevation_producers_);
}