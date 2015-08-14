#include "dem/elevation_cursor.h"

using elevations::dem::elevation_cursor;

elevation_cursor::cursor_task::cursor_task(ptr<elevation_cursor> elevation_cursor, const char* type, unsigned deadline)
	: Task(type, false, deadline)
	, elevation_cursor_(elevation_cursor)
{
	assert(elevation_cursor_ != nullptr && elevation_cursor_->task_graph_ != nullptr);
	elevation_cursor_->task_graph_->addTask(this);
}

ptr<elevations::dem::location> elevation_cursor::cursor_task::get_location() const
{
	return elevation_cursor_->location_;
}

void elevation_cursor::cursor_task::add_subtask(ptr<Task> task)
{
	assert(task != nullptr);
	auto task_graph = elevation_cursor_->task_graph_;
	task_graph->addTask(this);
	task_graph->addDependency(task, this);
	elevation_cursor_->reschedule();
}

elevation_cursor::set_location_task::set_location_task(const elevations::math::lat_lon_d& lat_lon, ptr<elevation_cursor> elevation_cursor, unsigned deadline)
	: cursor_task(elevation_cursor, "SetPositionTask", deadline)
	, lat_lon_(lat_lon)
{
}

bool elevation_cursor::set_location_task::run()
{
	elevation_cursor_->lat_lon_ = lat_lon_;
	elevation_cursor_->current_height_ = 0.0f;
	elevation_cursor_->location_ = elevation_cursor_->cube_mapper_->to_location(lat_lon_);

	add_subtask(new get_height_task(get_location(), elevation_cursor_, getDeadline()));

	return true;
}

elevation_cursor::set_level_task::set_level_task(size_t level, ptr<elevation_cursor> elevation_cursor, unsigned deadline)
	: cursor_task(elevation_cursor, "SetLevelTask", deadline)
	, level_(level)
{
}

bool elevation_cursor::set_level_task::run()
{
	if (level_ > get_location()->set_level(level_))
	{
		add_subtask(new get_height_task(get_location(), elevation_cursor_, getDeadline()));
	}

	return true;
}

elevation_cursor::get_height_task::get_height_task(ptr<elevations::dem::location> location, ptr<elevation_cursor> elevation_cursor, unsigned deadline)
	: cursor_task(elevation_cursor, "GetHeightTask", deadline)
{
	location_.reset(new elevations::dem::location(*location));

	auto tile = location_->get_tile(deadline);
	assert(tile != nullptr);

	auto task_graph = elevation_cursor_->task_graph_;

	auto task = tile->task;
	assert(task != nullptr);
	if (!task->isDone())
	{
		task_graph->addTask(task);
		task_graph->addDependency(this, task);
	}
}

bool elevation_cursor::get_height_task::run()
{
	elevation_cursor_->current_height_ = location_->get_height();

	return true;
}

void elevation_cursor::get_height_task::setIsDone(bool done, unsigned t, reason r)
{
	if (done)
	{
		location_->put_tile();
	}
	Task::setIsDone(done, t, r);
}

elevation_cursor::elevation_cursor(ptr<elevations::dem::cube_mapper> cube_mapper)
	: Object("ElevationCursor")
	, lat_lon_()
	, current_height_(0.0f)
	, cube_mapper_(cube_mapper)
	, task_graph_(new TaskGraph())
{
	assert(cube_mapper != nullptr);
	location_ = cube_mapper_->to_location(lat_lon_);
}

void elevation_cursor::set_position(const elevations::math::lat_lon_d& lat_lon)
{
	ptr<Task> task = new set_location_task(lat_lon, this);
	reschedule();
}

double elevation_cursor::get_current_height() const
{
	return current_height_;
}

void elevation_cursor::leave_request(size_t level)
{
	ptr<Task> task = new set_level_task(level, this);
	reschedule();
}

void elevation_cursor::reschedule() const
{
	location_->schedule(task_graph_);
}