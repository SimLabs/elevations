#pragma once

#include <algorithm>

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
			bool is_equal(T x, T y)
			{
				return std::abs(x - y) < lat_lon<T>::EPSILON;
			}

			template <typename T>
			bool both_are_max(T x, T y, T max)
			{
				return is_equal(x, y) && is_equal(x, max);
			}
		} // utils

		template <typename T>
		class lat_lon
		{
		public:
			explicit lat_lon(T latitude = T(), T longitude = T());
			lat_lon(const lat_lon<T>& other) = default;
			
			lat_lon<T>& operator=(const lat_lon<T>& other) = default;

			void swap(lat_lon<T>& other);

			vec3<T> to_cartesian() const;
			int determine_cube_face() const;			

			static const T LATITUDE_MAX_VALUE;
			static const T LONGITUDE_MAX_VALUE;
			static const T DEGREES_TO_RADIANS_COEFFICIENT;

			static const T EPSILON;

		private:
			T latitude_, longitude_, latitude_radians_, longitude_radians_;

			lat_lon(T latitude, T longitude, T latitude_radians, T longitude_radians);
		};

		template <typename T>
		lat_lon<T>::lat_lon(T latitude, T longitude, T latitude_radians, T longitude_radians)
			: latitude_(latitude)
			, longitude_(longitude)
			, latitude_radians_(latitude_radians)
			, longitude_radians_(longitude_radians)
		{
			assert(std::abs(latitude_) <= LATITUDE_MAX_VALUE && std::abs(longitude_) <= LONGITUDE_MAX_VALUE);
		}

		template <typename T>
		lat_lon<T>::lat_lon(T latitude, T longitude)
			: lat_lon(latitude, longitude, details::to_radians(latitude), details::to_radians(longitude))
		{
		}

		template <typename T>
		void lat_lon<T>::swap(lat_lon<T>& other)
		{
			using std::swap;
			swap(latitude_, other.latitude_);
			swap(longitude_, other.longitude_);
			swap(latitude_radians_, other.latitude_radians_);
			swap(longitude_radians_, other.longitude_radians_);
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
				return -1;
			}
			if (cartesian.z == max)
			{
				return 0;
			}
			if (-cartesian.y == max)
			{
				return 1;
			}
			if (cartesian.x == max)
			{
				return 2;
			}
			if (cartesian.y == max)
			{
				return 3;
			}
			if (-cartesian.x == max)
			{
				return 4;
			}
			if (-cartesian.z == max)
			{
				return 5;
			}
			assert(false);
			return -1;
		}

		template <typename T>
		const T lat_lon<T>::LATITUDE_MAX_VALUE = 90.0f;

		template <typename T>
		const T lat_lon<T>::LONGITUDE_MAX_VALUE = 180.0f;

		template <typename T>
		const T lat_lon<T>::DEGREES_TO_RADIANS_COEFFICIENT = M_PI / LONGITUDE_MAX_VALUE;

		typedef lat_lon<float> lat_lon_f;
		typedef lat_lon<double> lat_lon_d;

		const float lat_lon_f::EPSILON = 1e-6;
		const double lat_lon_d::EPSILON = 1e-9;
	} // primitives
} // project namespace
