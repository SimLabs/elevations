#pragma once

#include <array>

#include <proland/dem/CPUElevationProducer.h>

namespace elevations
{
	namespace dem
	{
		class cube_mapper : public Object
		{
		public:
			explicit cube_mapper();

			ptr<proland::CPUElevationProducer> operator[](size_t index);

			static const size_t FACES_NUMBER = 6;
			typedef std::array<ptr<proland::CPUElevationProducer>, FACES_NUMBER> elevation_producers;

		protected:
			elevation_producers elevation_producers_;

			void swap(ptr<cube_mapper> other);
		};
	} // digital elevation model
} // project namespace
