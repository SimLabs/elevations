#pragma once

#include <proland/dem/CPUElevationProducer.h>

#include <queries/dem/lat_lon_converter.h>

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
		class QUERIES_API elevation_cursor
		{
		public:
			class QUERIES_API cursor_state
			{
			public:
				explicit cursor_state(ptr<lat_lon_converter> lat_lon_converter);

			private:
				math::lat_lon_d lat_lon_;
				location location_;

				double height_;
				double precision_;
				size_t level_;				

				friend class elevation_cursor;
				friend class taskgraph::cursor_task;
				friend class taskgraph::set_location_task;
				friend class taskgraph::set_level_task;
				friend class taskgraph::get_height_task;
			};

			explicit elevation_cursor(lat_lon_converter* lat_lon_converter_);
			virtual ~elevation_cursor();

			void set_position(const math::lat_lon_d& lat_lon);
			double get_current_height() const;
			void leave_request(size_t level);

		private:
			cursor_state cursor_state_;

			lat_lon_converter* lat_lon_converter_;
			TaskGraph* task_graph_;
		};
	} // digital elevation model
} // project namespace