#include "taskgraph/get_height_task.h"

using elevations::taskgraph::get_height_task;

get_height_task::get_height_task(ptr<elevations::dem::elevation_cursor::cursor_state> cursor_state, ptr<TaskGraph> task_graph, unsigned deadline)
	: cursor_task(cursor_state, task_graph, "GetHeightTask", deadline)
{
	location_.reset(new dem::location(*cursor_state_->location_));

	auto tile = location_->get_tile(deadline);
	assert(tile != nullptr);

	auto task = tile->task;
	assert(task != nullptr);
	if (!task->isDone())
	{
		task_graph_->addTask(task);
		task_graph_->addDependency(this, task);
	}
}

bool get_height_task::run()
{
	cursor_state_->current_height_ = location_->get_height();
	return true;
}

void get_height_task::setIsDone(bool done, unsigned t, reason r)
{
	if (done)
	{
		location_->put_tile();
	}
	Task::setIsDone(done, t, r);
}