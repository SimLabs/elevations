#pragma once

#include <pmath.h>

#include <ork/math/vec3.h>

namespace elevations
{
	namespace math
	{
		template <typename T> class lat_lon;

		namespace details
		{
			template <typename T>
			T to_radians(T degrees)
			{
				return degrees * lat_lon<T>::DEGREES_TO_RADIANS_COEFFICIENT;
			}

			template <typename T>
			vec3<T> abs(const vec3<T>& vec)
			{
				return vec3<T>(std::abs(vec.x), std::abs(vec.y), std::abs(vec.z));
			}

			template <typename T>
			bool both_are_max(T x, T y, T max)
			{
				return x == y && x == max;
			}
		} // utils

		template <typename T>
		class lat_lon
		{
		public:
			lat_lon(T latitude, T longitude);

			vec3<T> to_cartesian() const;
			int determine_cube_face() const;

			const T latitude_, longitude_;

			static const T LATITUDE_MAX_VALUE;
			static const T LONGITUDE_MAX_VALUE;
			static const T DEGREES_TO_RADIANS_COEFFICIENT;

		private:
			const T latitude_radians_, longitude_radians_;
		};

		template <typename T>
		lat_lon<T>::lat_lon(T latitude, T longitude)
			: latitude_(latitude)
			, longitude_(longitude)
			, latitude_radians_(details::to_radians(latitude))
			, longitude_radians_(details::to_radians(longitude))
		{
			assert(std::abs(latitude) <= LATITUDE_MAX_VALUE && std::abs(longitude) <= LONGITUDE_MAX_VALUE);
		}

		template <typename T>
		vec3<T> lat_lon<T>::to_cartesian() const
		{
			auto latitude_cos = std::cos(latitude_radians_);
			return vec3<T>(latitude_cos * std::cos(longitude_radians_),
				latitude_cos * std::sin(longitude_radians_),
				std::sin(latitude_radians_));
		}

		template <typename T>
		int lat_lon<T>::determine_cube_face() const
		{
			auto cartesian = to_cartesian();
			auto cartesian_abs = details::abs(cartesian);
			auto max = std::max({ cartesian_abs.x, cartesian_abs.y, cartesian_abs.z });

			using details::both_are_max;
			if (both_are_max(cartesian_abs.x, cartesian_abs.y, max) ||
				both_are_max(cartesian_abs.y, cartesian_abs.z, max) ||
				both_are_max(cartesian_abs.x, cartesian_abs.z, max))
			{
				return 0;
			}
			if (cartesian.z == max)
			{
				return 1;
			}
			if (-cartesian.y == max)
			{
				return 2;
			}
			if (cartesian.x == max)
			{
				return 3;
			}
			if (cartesian.y == max)
			{
				return 4;
			}
			if (-cartesian.x == max)
			{
				return 5;
			}
			if (-cartesian.z == max)
			{
				return 6;
			}
			assert(false);
			return 0;
		}

		template <typename T>
		const T lat_lon<T>::LATITUDE_MAX_VALUE = 90.0f;

		template <typename T>
		const T lat_lon<T>::LONGITUDE_MAX_VALUE = 180.0f;

		template <typename T>
		const T lat_lon<T>::DEGREES_TO_RADIANS_COEFFICIENT = M_PI / LONGITUDE_MAX_VALUE;

		typedef lat_lon<float> lat_lon_f;
		typedef lat_lon<double> lat_lon_d;		
	} // primitives
} // project namespace
