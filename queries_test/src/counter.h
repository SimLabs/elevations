#pragma once

class counter
{
public:
	counter();	

	void start();
	double stop();

private:
	__int64 timestamp_;
	double frequency_;
};
