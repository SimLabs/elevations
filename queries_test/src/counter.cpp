#include "counter.h"

#include <windows.h>

#include <ork/core/Object.h>

counter::counter()
	: timestamp_(0)
	, frequency_(0)
{
	LARGE_INTEGER i;
	assert(QueryPerformanceFrequency(&i));
	frequency_ = double(i.QuadPart) / 1000.0;
}

void counter::start()
{
	LARGE_INTEGER i;
	QueryPerformanceCounter(&i);
	timestamp_ = i.QuadPart;
}

double counter::stop()
{
	LARGE_INTEGER i;
	QueryPerformanceCounter(&i);

	auto timestamp = timestamp_;
	timestamp_ = i.QuadPart;
	return double(timestamp_ - timestamp) / frequency_;
}