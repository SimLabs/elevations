#include <queries/dem/elevation_cursor.h>

#include <queries/taskgraph/set_location_task.h>
#include <queries/taskgraph/set_level_task.h>

using elevations::dem::elevation_cursor;

elevation_cursor::cursor_state::cursor_state(ptr<elevations::dem::lat_lon_converter> lat_lon_converter)
	: lat_lon_()
	, location_(lat_lon_converter->to_location(lat_lon_))
	, height_(0.0f)
	, precision_(0.0f)
	, level_(0)
{
}

elevation_cursor::elevation_cursor(elevations::dem::lat_lon_converter* lat_lon_converter)
	: Object("ElevationCursor")
	, cursor_state_(cursor_state(lat_lon_converter))
	, lat_lon_converter_(lat_lon_converter)
	, task_graph_(new TaskGraph())
{	
}

elevation_cursor::~elevation_cursor()
{
}

void elevation_cursor::set_position(const elevations::math::lat_lon_d& lat_lon)
{
	new taskgraph::set_location_task(lat_lon, lat_lon_converter_, cursor_state_, task_graph_);
}

double elevation_cursor::get_current_height() const
{
	return cursor_state_.height_;
}

void elevation_cursor::leave_request(size_t level)
{
	new taskgraph::set_level_task(level, cursor_state_, task_graph_);
}