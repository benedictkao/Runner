#pragma once

#include <chrono>
#include <iostream>
#include <string>

namespace debug {

#ifndef NDEBUG

	template <typename... Args>
	void log(const char* format, Args... args) {
		std::string formatAsString = "[%s] ";
		formatAsString += format;
		formatAsString += "\n";
		const time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		printf(formatAsString.c_str(), strtok(ctime(&currentTime), "\n"), args...);
	}

#else

	template <typename... Args>
	void log(const char* format, Args... args) {}

#endif
}