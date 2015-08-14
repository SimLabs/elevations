#pragma once

#include <proland/dem/CPUElevationProducer.h>

#include "dem/cube_mapper.h"

namespace elevations
{
	namespace dem
	{
		class elevation_cursor : public Object
		{
			class cursor_task : public Task
			{
			protected:
				cursor_task(ptr<elevation_cursor> elevation_cursor, const char* type, unsigned deadline = 0);

				ptr<location> get_location() const;
				void add_subtask(ptr<Task> task);

				ptr<elevation_cursor> elevation_cursor_;
			};

			class set_location_task : public cursor_task
			{
			public:
				set_location_task(const math::lat_lon_d& lat_lon, ptr<elevation_cursor> elevation_cursor, unsigned deadline = 0);
				bool run() override;

			private:
				math::lat_lon_d lat_lon_;
			};

			class set_level_task : public cursor_task
			{
			public:
				set_level_task(size_t level, ptr<elevation_cursor> elevation_cursor, unsigned deadline = 0);
				bool run() override;

			private:
				size_t level_;
			};

			class get_height_task : public cursor_task
			{
			public:
				explicit get_height_task(ptr<location> location, ptr<elevation_cursor> elevation_cursor, unsigned deadline = 0);
				bool run() override;
				void setIsDone(bool done, unsigned int t, reason r = DATA_NEEDED) override;

			private:
				ptr<location> location_;
			};

		public:
			explicit elevation_cursor(ptr<cube_mapper> cube_mapper);

			void set_position(const math::lat_lon_d& lat_lon);
			double get_current_height() const;
			void leave_request(size_t level);

		private:
			math::lat_lon_d lat_lon_;
			double current_height_;
			
			ptr<cube_mapper> cube_mapper_;			
			ptr<TaskGraph> task_graph_;
			ptr<location> location_;

			void reschedule() const;
		};
	} // digital elevation model
} // project namespace