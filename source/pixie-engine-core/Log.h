#pragma once
#include "pch.h"
#include <cstdio>

class Log {
public:
	template<typename... Args>
	inline static void Message(const std::string& message, Args... args) {
		printf((message + "\n").c_str(), args...);
	}

	template<typename... Args>
	inline static void Warning(const std::string& warning, Args... args) {
		printf((warning + "\n").c_str(), args...);
	}

	template<typename... Args>
	inline static void Error(const std::string& error, Args... args) {
		printf((error + "\n").c_str(), args...);
	}
};

class DebugLog {
public:
	template<typename... Args>
	static void Message(const std::string& message, Args... args) {
		printf((message + "\n").c_str(), args...);
	}

	template<typename... Args>
	static void Warning(const std::string& warning, Args... args) {
		printf((warning + "\n").c_str(), args...);
	}

	template<typename... Args>
	static void Error(const std::string& error, Args... args) {
		printf((error + "\n").c_str(), args...);
	}
};
