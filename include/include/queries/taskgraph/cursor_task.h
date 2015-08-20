#pragma once

#include <ork/taskgraph/Task.h>

#include <queries/dem/elevation_cursor.h>

namespace elevations
{
	namespace taskgraph
	{
		class QUERIES_API cursor_task : public Task
		{
		protected:
			cursor_task(dem::elevation_cursor::cursor_state& cursor_state, ptr<TaskGraph> task_graph, const char* type, unsigned deadline = 0);

			virtual void setIsDone(bool done, unsigned int t, reason r = DATA_NEEDED) override;
			void add_subtask(ptr<Task> task);

			dem::elevation_cursor::cursor_state& cursor_state_;
			ptr<TaskGraph> task_graph_;

		private:
			void reschedule() const;
		};
	} // tasks
} // project namespace