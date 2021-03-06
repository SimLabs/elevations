﻿#include <queries/taskgraph/set_location_task.h>

#include <queries/taskgraph/get_height_task.h>

using elevations::taskgraph::set_location_task;

set_location_task::set_location_task(const elevations::math::lat_lon_d& lat_lon, ptr<elevations::dem::lat_lon_converter> lat_lon_converter, elevations::dem::elevation_cursor::cursor_state& cursor_state, ptr<TaskGraph> task_graph, unsigned deadline)
	: cursor_task(cursor_state, task_graph, "SetPositionTask", deadline)
	, lat_lon_(lat_lon)
	, lat_lon_converter_(lat_lon_converter)
{
}

set_location_task::~set_location_task()
{
}

bool set_location_task::run()
{
	cursor_state_.lat_lon_ = lat_lon_;
	cursor_state_.height_ = 0.0f;
	cursor_state_.location_ = lat_lon_converter_->to_location(lat_lon_);

	add_subtask(new get_height_task(true, cursor_state_, task_graph_, getDeadline()));

	return true;
}