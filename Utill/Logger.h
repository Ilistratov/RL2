#pragma once

#include <iostream>
#include <sstream>
#include <fstream>

#include <vector>
#include <string>

class Logger {
	std::ofstream logFileOut;
public:
	const static uint8_t LogLvl;

	Logger& operator=(Logger const&) = delete;
	Logger(Logger const&) = delete;

	Logger() {}
	bool startSession(const std::string& logFilePath);
	void stopSession();

	enum class Color {
		Black = 30,
		Red = 31,
		Green = 32,
		Yellow = 33,
		Blue = 34,
		Magenta = 35,
		Cyan = 36,
		White = 37,
		Reset = 0
	};

	static void addTime(std::stringstream& ss);
	static void setColor(std::stringstream& ss, Color color);
	static void addColoredText(std::stringstream& ss, const std::string& text, Color color);
	static void addTag(std::stringstream& ss, const std::string& text, Color color);

	template<typename T>
	static void addVector(std::stringstream& ss, const std::vector<T>& vec);

	static void addDebugTag(std::stringstream& ss);
	static void addWarningTag(std::stringstream& ss);
	static void addErrorTag(std::stringstream& ss);
	static void addInfoTag(std::stringstream& ss);

	void rawMsg(const std::string& msg);

	void debugMsg(const std::string& msg);
	void warningMsg(const std::string& msg);
	void errorMsg(const std::string& msg);
	void infoMsg(const std::string& msg);
};

extern Logger GlobalLog;

template<typename T>
inline void Logger::addVector(std::stringstream& ss, const std::vector<T>& vec) {
	ss << "Count: " << vec.size() << '\n';
	for (const auto& el : vec) {
		ss << el << '\n';
	}
}
