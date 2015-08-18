#pragma once

#include <ork/taskgraph/Task.h>

#include "dem/elevation_cursor.h"

namespace elevations
{
	namespace taskgraph
	{
		class cursor_task : public Task
		{
		protected:
			cursor_task(ptr<dem::elevation_cursor::cursor_state> cursor_state, ptr<TaskGraph> task_graph, const char* type, unsigned deadline = 0);

			void add_subtask(ptr<Task> task);

			ptr<dem::elevation_cursor::cursor_state> cursor_state_;
			ptr<TaskGraph> task_graph_;

		private:
			void reschedule() const;
		};
	} // tasks
} // project namespace