#pragma once
#include <filesystem>
#include <fstream>
#include <shared_mutex>

struct Logger
{
	Logger() = default;
	~Logger() = default;
	void toLog(const std::string& text);
	std::string fromLog();
	std::shared_mutex sd;
};
