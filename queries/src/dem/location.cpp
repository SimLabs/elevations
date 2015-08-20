#include <queries/dem/location.h>

using elevations::dem::location;

location::location(double x, double y, proland::CPUElevationProducer* elevation_producer)
	: x_(x)
	, y_(y)
	, level_(0)
	, tx_(0)
	, ty_(0)
	, elevation_producer_(elevation_producer)
{
	assert(elevation_producer_ != nullptr);
	set_level(elevation_producer_->getMinLevel());
}

vec2d location::get_height_with_precision() const
{
	auto result = proland::CPUElevationProducer::getHeightWithPrecision(elevation_producer_, level_, x_, y_);
	return vec2d(result.x, result.y);
}

size_t location::get_level() const
{
	return level_;
}

void location::set_level(size_t level)
{
	assert(elevation_producer_->getMinLevel() <= level && level <= elevation_producer_->getMaxLevel());
	level_ = level;

	auto quad_size = elevation_producer_->getRootQuadSize();
	tx_ = details::physical_to_logical(x_, quad_size, level);
	ty_ = details::physical_to_logical(y_, quad_size, level);
}

proland::TileCache::Tile* location::get_tile(unsigned deadline) const
{
	return elevation_producer_->getTile(level_, tx_, ty_, deadline);
}

void location::put_tile() const
{
	elevation_producer_->putTile(elevation_producer_->findTile(level_, tx_, ty_));
}

void location::schedule(ptr<TaskGraph> task_graph) const
{
	elevation_producer_->schedule(task_graph);
}

int elevations::dem::details::physical_to_logical(double value, double quad_size, int level)
{
	auto result = value / quad_size + 0.5f;
	return result >= 0.0f && result <= 1.0f ? result * (1 << level) : -1;
}