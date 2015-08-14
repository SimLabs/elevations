#pragma once

#include "producer/height_layer.h"

namespace elevations
{
	namespace dem
	{
		class location : public Object
		{
		public:
			explicit location(double x, double y, ptr<producer::height_layer> height_layer);

			double get_height() const;
			proland::TileCache::Tile* get_tile(unsigned deadline = 0) const;
			void put_tile() const;

			size_t set_level(size_t level);

			void schedule(ptr<TaskGraph> task_graph) const;

		private:
			double x_, y_;
			size_t level_;
			int tx_, ty_;

			ptr<producer::height_layer> height_layer_;
		};

		namespace details
		{
			int physical_to_logical(double value, double quad_size, int level);
		} // utils
	} // digital elevation model
} // project namespace