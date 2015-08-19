#include "taskgraph/get_height_task.h"

using elevations::taskgraph::get_height_task;

get_height_task::get_height_task(bool forced, elevations::dem::elevation_cursor::cursor_state& cursor_state, ptr<TaskGraph> task_graph, unsigned deadline)
	: cursor_task(cursor_state, task_graph, "GetHeightTask", deadline)
	, forced_(forced)
	, location_(cursor_state.location_)
{
	auto tile = location_.get_tile(deadline);
	assert(tile != nullptr);

	auto tile_task = tile->task;
	assert(tile_task != nullptr);
	if (!tile_task->isDone())
	{
		task_graph_->addTask(tile_task);
		task_graph_->addDependency(this, tile_task);
	}
}

bool get_height_task::run()
{
	auto level = location_.get_level();
	if (forced_ || level > cursor_state_.level_)
	{
		auto vector = location_.get_height_with_precision();
		cursor_state_.height_ = vector.x;
		cursor_state_.precision_ = vector.y;
		cursor_state_.level_ = level;
	}
	return true;
}

void get_height_task::setIsDone(bool done, unsigned t, reason r)
{
	if (done)
	{
		location_.put_tile();
	}
	cursor_task::setIsDone(done, t, r);
}