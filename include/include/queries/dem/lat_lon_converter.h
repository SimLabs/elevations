#pragma once

#include <array>

#include <proland/dem/CPUElevationProducer.h>
#include <proland/terrain/SphericalDeformation.h>

#include <queries/math/lat_lon.h>
#include <queries/dem/location.h>

namespace elevations
{
	namespace dem
	{
		class QUERIES_API lat_lon_converter: public Object
		{
		public:
			explicit lat_lon_converter(float radius = 1.0f);
			void init(float radius);

			virtual ~lat_lon_converter();

			location to_location(const math::lat_lon_d& lat_lon) const;

		private:
			ptr<proland::SphericalDeformation> spherical_deformation_;			

		protected:
			static const size_t FACES_NUMBER = 6;
			std::array<ptr<proland::CPUElevationProducer>, FACES_NUMBER> elevation_producers_;

			void swap(ptr<lat_lon_converter> other);
		};
	} // digital elevation model
} // project namespace
