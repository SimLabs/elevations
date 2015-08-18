#pragma once

#include <array>

#include <proland/dem/CPUElevationProducer.h>
#include <proland/terrain/SphericalDeformation.h>

#include "producer/height_layer.h"
#include "math/lat_lon.h"
#include "dem/location.h"

namespace elevations
{
	namespace dem
	{
		class lat_lon_converter : public Object
		{
		public:
			explicit lat_lon_converter(float radius = 1.0f);
			void init(float radius);

			ptr<location> to_location(const math::lat_lon_d& lat_lon) const;

		private:
			ptr<proland::SphericalDeformation> spherical_deformation_;			

		protected:
			static const size_t FACES_NUMBER = 6;
			std::array<ptr<producer::height_layer>, FACES_NUMBER> height_layers_;

			void swap(ptr<lat_lon_converter> other);
		};
	} // digital elevation model
} // project namespace
