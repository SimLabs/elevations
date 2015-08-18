#pragma once

#include "taskgraph/cursor_task.h"

namespace elevations
{
	namespace taskgraph
	{
		class set_location_task : public cursor_task
		{
		public:
			set_location_task(const math::lat_lon_d& lat_lon, ptr<dem::lat_lon_converter> lat_lon_converter,
				ptr<dem::elevation_cursor::cursor_state> cursor_state_, ptr<TaskGraph> task_graph, unsigned deadline = 0);
			bool run() override;

		private:
			math::lat_lon_d lat_lon_;

			ptr<dem::lat_lon_converter> lat_lon_converter_;
		};
	} // tasks
} // project namespace