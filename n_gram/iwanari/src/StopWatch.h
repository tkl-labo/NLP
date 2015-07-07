#pragma once
#include <iostream>
#include <chrono>
#include <sstream>

namespace util {
template<typename TimeT = std::chrono::milliseconds, 
	typename ClockT=std::chrono::high_resolution_clock,
	typename DurationT=double>
class Stopwatch {
private:
	std::chrono::time_point<ClockT> _start, _end;
public:
	Stopwatch() { start(); }
	void start() { _start = _end = ClockT::now(); }
	DurationT stop() { _end = ClockT::now(); return elapsed();}
	DurationT elapsed() { 
		auto delta = std::chrono::duration_cast<TimeT>(_end-_start);
		return delta.count(); 
	}
	std::string showElapsedTime() {
		std::stringstream ss;
		ss << elapsed() << " [" << showUnit(TimeT()) << "]";
		return ss.str();
	}
	std::string showUnit(std::chrono::seconds s) { return "s"; }
	std::string showUnit(std::chrono::milliseconds s) { return "ms"; }
	std::string showUnit(std::chrono::microseconds s) { return "mus"; }
	std::string showUnit(std::chrono::nanoseconds s) { return "ns"; }
};
}
