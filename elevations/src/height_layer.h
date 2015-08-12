#pragma once

#include <proland/producer/TileLayer.h>
#include <proland/dem/CPUElevationProducer.h>

namespace elevations
{
	class height_layer : public proland::TileLayer
	{
	public:
		explicit height_layer(bool deform = false);

		void setTileSize(int tileSize, int tileBorder, float rootQuadSize) override;
		bool doCreateTile(int level, int tx, int ty, proland::TileStorage::Slot* data) override;

	protected:
		ptr<proland::CPUElevationProducer> elevation_producer_;

		void swap(ptr<height_layer> other);
	};
} // project namespace
