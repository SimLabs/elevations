#include "elevation_cursor.h"

using elevations::elevation_cursor;

elevation_cursor::cursor_task::cursor_task(elevation_cursor* elevation_cursor, const char* type, unsigned deadline)
	: Task(type, false, deadline)
	, elevation_cursor_(elevation_cursor)
{
}

elevation_cursor::set_location_task::set_location_task(double x, double y, elevation_cursor* elevation_cursor, unsigned deadline)
	: cursor_task(elevation_cursor, "SetPositionTask", deadline)
	, x_(x)
	, y_(y)
{
}

bool elevation_cursor::set_location_task::run()
{
	elevation_cursor_->x_ = x_;
	elevation_cursor_->y_ = y_;

	auto level = elevation_cursor_->elevation_producer_->getMinLevel();
	auto quad_size = elevation_cursor_->elevation_producer_->getRootQuadSize();
	elevation_cursor_->tx_ = details::physical_to_logical(x_, quad_size, level);
	elevation_cursor_->ty_ = details::physical_to_logical(y_, quad_size, level);

	return true;
}

elevation_cursor::set_level_task::set_level_task(int level, elevation_cursor* elevation_cursor, unsigned deadline)
	: cursor_task(elevation_cursor, "SetLevelTask", deadline)
	, level_(level)
{
	auto tile = elevation_cursor_->elevation_producer_->getTile(level, elevation_cursor_->tx_, elevation_cursor_->ty_, getDeadline());
	assert(tile != nullptr);
}

bool elevation_cursor::set_level_task::run()
{
	elevation_cursor_->current_height_ = proland::CPUElevationProducer::getHeight(elevation_cursor_->elevation_producer_,
		elevation_cursor_->level_,
		elevation_cursor_->x_,
		elevation_cursor_->y_);

	return true;
}

elevation_cursor::elevation_cursor(ptr<proland::CPUElevationProducer> elevation_producer, ptr<TaskGraph> task_graph)
	: Object("elevationCursor")
	, elevation_producer_(elevation_producer)
	, task_graph_(task_graph != nullptr ? task_graph : new TaskGraph())
	, x_(0.0f)
	, y_(0.0f)
	, current_height_(0.0f)
	, level_(0)
	, tx_(0)
	, ty_(0)
{
	assert(elevation_producer_ != nullptr);
	level_ = elevation_producer_->getMinLevel();
}

void elevation_cursor::set_position(double x, double y)
{
	ptr<Task> task = new set_location_task(x, y, this);
	task_graph_->addTask(task);
	schedule_task(task);
}

double elevation_cursor::get_current_height() const
{
	return current_height_;
}

void elevation_cursor::leave_request(int level)
{
	ptr<Task> task = new set_level_task(level, this);
	task_graph_->addTask(task);
	schedule_task(task);
}

void elevation_cursor::schedule_task(ptr<Task> task) const
{
	elevation_producer_->getCache()->getScheduler()->schedule(task);
}

int elevations::details::physical_to_logical(double value, double quad_size, int level)
{
	assert(quad_size != 0);
	auto result = value / quad_size + 0.5f;
	return result >= 0.0f && result <= 1.0f ? result * (1 << level) : -1;
}