#include <iomanip>
#include <sstream>
#include <chrono>

#include "Logger.h"

bool Logger::startSession(const std::string& logFilePath) {
	if (logFileOut.is_open()) {
		logFileOut.close();
	}

	logFileOut.open(logFilePath);
	return logFileOut.is_open();
}

void Logger::stopSession() {
	if (logFileOut.is_open()) {
		logFileOut.close();
	}
}

void Logger::addTime(std::stringstream& ss) {
	ss << '[';
	std::time_t tt = std::time(nullptr);
	ss << std::put_time(std::localtime(&tt), "%m/%d %T");

	ss << "] ";
}

void Logger::setColor(std::stringstream& ss, Color color) {
	ss << "\u001b[" << (int)color << 'm';
}

void Logger::addColoredText(std::stringstream& ss, const std::string& text, Color color) {
	setColor(ss, color);
	ss << text;
	setColor(ss, Color::Reset);
}

void Logger::addTag(std::stringstream& ss, const std::string& text, Color color) {
	ss << '[';
	addColoredText(ss, text, color);
	ss << "] ";
}

void Logger::addDebugTag(std::stringstream& ss) {
	addTag(ss, "Debug", Color::Cyan);
}

void Logger::addWarningTag(std::stringstream& ss) {
	addTag(ss, "Warning", Color::Yellow);
}

void Logger::addErrorTag(std::stringstream& ss) {
	addTag(ss, "Error", Color::Red);
}

void Logger::addInfoTag(std::stringstream& ss) {
	addTag(ss, "Info", Color::Green);
}

void Logger::rawMsg(const std::string& msg) {
	std::cout << msg;
	if (!logFileOut.is_open() || logFileOut.bad()) {
		//std::cout << "unable to write to log file\n";
	} else {
		logFileOut << msg;
	}
}

void Logger::debugMsg(const std::string& msg) {
	if (LogLvl > 0) {
		return;
	}

	std::stringstream ss;
	addTime(ss);
	addDebugTag(ss);
	ss << msg << '\n';
	rawMsg(ss.str());
}

void Logger::warningMsg(const std::string& msg) {
	if (LogLvl > 2) {
		return;
	}

	std::stringstream ss;
	addTime(ss);
	addWarningTag(ss);
	ss << msg << '\n';
	rawMsg(ss.str());
}

void Logger::errorMsg(const std::string& msg) {
	if (LogLvl > 3) {
		return;
	}

	std::stringstream ss;
	addTime(ss);
	addErrorTag(ss);
	ss << msg << '\n';
	rawMsg(ss.str());
}

void Logger::infoMsg(const std::string& msg) {
	if (LogLvl > 1) {
		return;
	}

	std::stringstream ss;
	addTime(ss);
	addInfoTag(ss);
	ss << msg << '\n';
	rawMsg(ss.str());
}

Logger GlobalLog;
const uint8_t Logger::LogLvl = 0;