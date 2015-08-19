#pragma once

#include <ork/math/mat3.h>
#include <proland/producer/TileLayer.h>
#include <proland/dem/CPUElevationProducer.h>

namespace elevations
{
	namespace producer
	{
		class height_layer : public proland::TileLayer
		{
		public:
			explicit height_layer();

			size_t get_min_level() const;
			size_t get_max_level() const;

			vec2f get_height_with_precision(size_t level, double x, double y) const;

			proland::TileCache::Tile* get_tile(size_t level, int tx, int ty, unsigned deadline) const;
			proland::TileCache::Tile* find_tile(size_t level, int tx, int ty) const;
			void put_tile(proland::TileCache::Tile* tile) const;

			vec3d rotate(const vec3d& vector) const;

			void schedule(ptr<TaskGraph> task_graph) const;

			void setTileSize(int tileSize, int tileBorder, float rootQuadSize) override;
			bool doCreateTile(int level, int tx, int ty, proland::TileStorage::Slot* data) override;			

		protected:
			mat3d rotation_matrix_;

			ptr<proland::CPUElevationProducer> elevation_producer_;

			void swap(ptr<height_layer> other);
		};
	} // producers
} // project namespace
