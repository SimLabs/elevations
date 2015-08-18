#pragma once

#include <proland/dem/CPUElevationProducer.h>

#include "dem/lat_lon_converter.h"

namespace elevations
{
	namespace taskgraph
	{
		class cursor_task;
		class set_location_task;
		class set_level_task;
		class get_height_task;
	} // tasks

	namespace dem
	{
		class elevation_cursor : public Object
		{
		public:
			class cursor_state : public Object
			{
			public:
				explicit cursor_state(ptr<lat_lon_converter> lat_lon_converter);

			private:
				math::lat_lon_d lat_lon_;
				double current_height_;

				ptr<location> location_;

				friend class elevation_cursor;
				friend class taskgraph::cursor_task;
				friend class taskgraph::set_location_task;
				friend class taskgraph::set_level_task;
				friend class taskgraph::get_height_task;
			};

			explicit elevation_cursor(ptr<lat_lon_converter> lat_lon_converter_);

			void set_position(const math::lat_lon_d& lat_lon);
			double get_current_height() const;
			void leave_request(size_t level);

		private:
			ptr<lat_lon_converter> lat_lon_converter_;
			ptr<TaskGraph> task_graph_;
			ptr<cursor_state> cursor_state_;
		};
	} // digital elevation model
} // project namespace