#pragma once

#include <proland/dem/CPUElevationProducer.h>

namespace elevations
{
	class elevation_cursor : public Object
	{
		class cursor_task : public Task
		{
		protected:
			cursor_task(elevation_cursor* elevation_cursor, const char* type, unsigned deadline = 0);

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

	public:

		explicit elevation_cursor(ptr<proland::CPUElevationProducer> elevation_producer, ptr<TaskGraph> task_graph = nullptr);

		void set_position(double x, double y);

		double get_current_height() const;

		void leave_request(int level);

	private:
		ptr<proland::CPUElevationProducer> elevation_producer_;
		ptr<TaskGraph> task_graph_;

		double x_, y_, current_height_;
		int level_, tx_, ty_;

		void schedule_task(ptr<Task> task) const;
	};

	namespace details
	{
		int physical_to_logical(double value, double quad_size, int level);
	} // utils
} // project namespace