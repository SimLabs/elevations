#pragma once

#include "taskgraph/cursor_task.h"

namespace elevations
{
	namespace taskgraph
	{
		class set_level_task : public cursor_task
		{
		public:
			set_level_task(size_t level, dem::elevation_cursor::cursor_state& cursor_state, ptr<TaskGraph> task_graph, unsigned deadline = 0);
			bool run() override;

		private:
			size_t level_;
		};
	} // tasks
} // project namespace
