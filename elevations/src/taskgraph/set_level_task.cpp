#include "taskgraph/set_level_task.h"

#include "taskgraph/get_height_task.h"

using elevations::taskgraph::set_level_task;

set_level_task::set_level_task(size_t level,
	ptr<elevations::dem::elevation_cursor::cursor_state> cursor_state, ptr<TaskGraph> task_graph, unsigned deadline)
	: cursor_task(cursor_state, task_graph, "SetLevelTask", deadline)
	, level_(level)
{
}

bool set_level_task::run()
{
	if (level_ > cursor_state_->location_->set_level(level_))
	{
		add_subtask(new get_height_task(cursor_state_, task_graph_, getDeadline()));
	}

	return true;
}