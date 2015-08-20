#pragma once

#include <proland/dem/CPUElevationProducer.h>

namespace elevations
{
	namespace dem
	{
		class QUERIES_API location
		{
		public:
			explicit location(double x, double y, proland::CPUElevationProducer* elevation_producer);

			vec2d get_height_with_precision() const;

			size_t get_level() const;
			void set_level(size_t level);

			proland::TileCache::Tile* get_tile(unsigned deadline = 0) const;
			void put_tile() const;

			void schedule(ptr<TaskGraph> task_graph) const;

		private:
			double x_, y_;
			size_t level_;
			int tx_, ty_;

			proland::CPUElevationProducer* elevation_producer_;
		};

		namespace details
		{
			int physical_to_logical(double value, double quad_size, int level);
		} // utils
	} // digital elevation model
} // project namespace