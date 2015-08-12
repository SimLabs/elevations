#include "elevation_cursor.h"

using elevations::elevation_cursor;

elevation_cursor::cursor_task::cursor_task(elevation_cursor* elevation_cursor, const char* type, unsigned deadline)
	: Task(type, false, deadline)
	, elevation_cursor_(elevation_cursor)
{
	elevation_cursor_->task_graph_->addTask(this);
}

void elevation_cursor::cursor_task::add_subtask(ptr<Task> task, bool before)
{
	auto task_graph = elevation_cursor_->task_graph_;
	task_graph->addTask(task);
	if (before)
	{
		task_graph->addDependency(this, task);
	}
	else
	{
		task_graph->addDependency(task, this);
	}

	elevation_cursor_->reschedule();
}

ptr<proland::CPUElevationProducer> elevation_cursor::cursor_task::get_elevation_producer() const
{
	return elevation_cursor_->elevation_producer_;
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
	elevation_cursor_->current_height_ = 0.0f;
	elevation_cursor_->set_level(get_elevation_producer()->getMinLevel());

	add_subtask(new get_height_task(elevation_cursor_, getDeadline()));

	return true;
}

elevation_cursor::set_level_task::set_level_task(int level, elevation_cursor* elevation_cursor, unsigned deadline)
	: cursor_task(elevation_cursor, "SetLevelTask", deadline)
	, level_(level)
{
	auto elevation_producer = get_elevation_producer();
	assert(elevation_producer->getMinLevel() <= level && level <= elevation_producer->getMaxLevel());
}

bool elevation_cursor::set_level_task::run()
{
	auto old_level = elevation_cursor_->level_;
	elevation_cursor_->set_level(level_);

	if (level_ > old_level)
	{
		add_subtask(new get_height_task(elevation_cursor_, getDeadline()));
	}

	return true;
}

elevation_cursor::get_height_task::get_height_task(elevation_cursor* elevation_cursor, unsigned deadline)
	: cursor_task(elevation_cursor, "GetHeightTask", deadline)
	, level_(elevation_cursor->level_)
	, tx_(elevation_cursor->tx_)
	, ty_(elevation_cursor->ty_)
	, x_(elevation_cursor->x_)
	, y_(elevation_cursor->y_)
{
	auto tile = get_elevation_producer()->getTile(level_, tx_, ty_, deadline);
	assert(tile != nullptr);

	auto task = tile->task;
	assert(task != nullptr);
	if (!task->isDone())
	{
		add_subtask(task, true);
	}
}

bool elevation_cursor::get_height_task::run()
{
	elevation_cursor_->current_height_ = proland::CPUElevationProducer::getHeight(get_elevation_producer(), level_, x_, y_);

	return true;
}

void elevation_cursor::get_height_task::setIsDone(bool done, unsigned t, reason r)
{
	Task::setIsDone(done, t, r);

	if (done)
	{
		auto elevation_producer = get_elevation_producer();
		elevation_producer->putTile(elevation_producer->findTile(level_, tx_, ty_, true));
	}
}

elevation_cursor::elevation_cursor(ptr<proland::CPUElevationProducer> elevation_producer)
	: Object("ElevationCursor")
	, elevation_producer_(elevation_producer)
	, task_graph_(new TaskGraph())
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
	reschedule();
}

double elevation_cursor::get_current_height() const
{
	return current_height_;
}

void elevation_cursor::leave_request(int level)
{
	ptr<Task> task = new set_level_task(level, this);
	reschedule();
}

void elevation_cursor::set_level(int level)
{
	auto quad_size = elevation_producer_->getRootQuadSize();
	level_ = level;
	tx_ = details::physical_to_logical(x_, quad_size, level);
	ty_ = details::physical_to_logical(y_, quad_size, level);
}

void elevation_cursor::reschedule() const
{
	elevation_producer_->getCache()->getScheduler()->schedule(task_graph_);
}

int elevations::details::physical_to_logical(double value, double quad_size, int level)
{
	assert(quad_size != 0);
	auto result = value / quad_size + 0.5f;
	auto temp = 1 << level;
	return result >= 0.0f && result <= 1.0f ? result * temp : -1;
}