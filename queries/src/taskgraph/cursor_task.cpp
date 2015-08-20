#include <queries/taskgraph/cursor_task.h>

using elevations::taskgraph::cursor_task;

cursor_task::cursor_task(elevations::dem::elevation_cursor::cursor_state& cursor_state, ptr<TaskGraph> task_graph, const char* type, unsigned deadline)
	: Task(type, false, deadline)
	, cursor_state_(cursor_state)
	, task_graph_(task_graph)
{
	assert(task_graph_ != nullptr);
	task_graph_->addTask(this);
	reschedule();
}

cursor_task::~cursor_task()
{
}

void cursor_task::setIsDone(bool done, unsigned t, reason r)
{
	task_graph_ = nullptr;
	Task::setIsDone(done, t, r);
}

void cursor_task::add_subtask(ptr<Task> task)
{
	assert(task != nullptr);
	task_graph_->addTask(this);
	task_graph_->addDependency(task, this);
	reschedule();
}

void cursor_task::reschedule() const
{
	cursor_state_.location_.schedule(task_graph_);
}