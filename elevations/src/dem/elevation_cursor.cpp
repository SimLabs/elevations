#include "dem/elevation_cursor.h"

#include "taskgraph/set_location_task.h"
#include "taskgraph/set_level_task.h"

using elevations::dem::elevation_cursor;

elevation_cursor::cursor_state::cursor_state(ptr<elevations::dem::lat_lon_converter> lat_lon_converter)
	: Object("CursorState")
	, lat_lon_()
	, current_height_(0.0f)
{
	assert(lat_lon_converter != nullptr);
	location_ = lat_lon_converter->to_location(lat_lon_);
}

elevation_cursor::elevation_cursor(ptr<elevations::dem::lat_lon_converter> lat_lon_converter)
	: Object("ElevationCursor")
	, lat_lon_converter_(lat_lon_converter)
	, task_graph_(new TaskGraph())
	, cursor_state_(new cursor_state(lat_lon_converter))
{	
}

void elevation_cursor::set_position(const elevations::math::lat_lon_d& lat_lon)
{
	new taskgraph::set_location_task(lat_lon, lat_lon_converter_, cursor_state_, task_graph_);
}

double elevation_cursor::get_current_height() const
{
	return cursor_state_->current_height_;
}

void elevation_cursor::leave_request(size_t level)
{
	new taskgraph::set_level_task(level, cursor_state_, task_graph_);
}