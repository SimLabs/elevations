#pragma once

#include <proland/dem/CPUElevationProducer.h>

namespace elevations
{
	namespace dem
	{
		class elevation_cursor : public Object
		{
			class cursor_task : public Task
			{
			protected:
				cursor_task(elevation_cursor* elevation_cursor, const char* type, unsigned deadline = 0);

				void add_subtask(ptr<Task> task, bool before = false);
				ptr<proland::CPUElevationProducer> get_elevation_producer() const;

				elevation_cursor* elevation_cursor_;
			};

			class set_location_task : public cursor_task
			{
			public:
				set_location_task(double x, double y, elevation_cursor* elevation_cursor, unsigned deadline = 0);
				bool run() override;

			private:
				double x_, y_;
			};

			class set_level_task : public cursor_task
			{
			public:
				set_level_task(int level, elevation_cursor* elevation_cursor, unsigned deadline = 0);
				bool run() override;

			private:
				int level_;
			};

			class get_height_task : public cursor_task
			{
			public:
				explicit get_height_task(elevation_cursor* elevation_cursor, unsigned deadline = 0);
				bool run() override;
				void setIsDone(bool done, unsigned int t, reason r = DATA_NEEDED) override;

			private:
				int level_, tx_, ty_;
				double x_, y_;
			};

		public:
			explicit elevation_cursor(ptr<proland::CPUElevationProducer> elevation_producer);

			void set_position(double x, double y);
			double get_current_height() const;
			void leave_request(int level);

		private:
			ptr<proland::CPUElevationProducer> elevation_producer_;
			ptr<TaskGraph> task_graph_;

			double x_, y_, current_height_;
			int level_, tx_, ty_;

			void set_level(int level);

			void reschedule() const;
		};

		namespace details
		{
			int physical_to_logical(double value, double quad_size, int level);
		} // utils
	} // digital elevation model
} // project namespace