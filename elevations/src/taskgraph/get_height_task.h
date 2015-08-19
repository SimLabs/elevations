#pragma once

#include "taskgraph/cursor_task.h"

namespace elevations
{
	namespace taskgraph
	{
		class get_height_task : public cursor_task
		{
		public:
			get_height_task(bool forced, dem::elevation_cursor::cursor_state& cursor_state, ptr<TaskGraph> task_graph, unsigned deadline = 0);
			bool run() override;
			void setIsDone(bool done, unsigned int t, reason r = DATA_NEEDED) override;

		private:
			bool forced_;
			dem::location location_;
		};
	} // tasks
} // project namespace