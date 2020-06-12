#pragma once
#include <time.h>
#include <string>
#include <iomanip>
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#endif

#define ldebug(...) Logger::color(); Logger::verbose(__func__,__LINE__, __VA_ARGS__)
#define linfo(...) Logger::color(); Logger::info(__func__,__LINE__, __VA_ARGS__)
#define lerror(...) Logger::color(); Logger::error(__func__,__LINE__, __VA_ARGS__)
#define lwarn(...) Logger::color(); Logger::warn(__func__,__LINE__, __VA_ARGS__)

namespace Logger
{
	inline std::time_t time_now = std::time(nullptr);

	inline void color()
	{
#ifdef _WIN32
		HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD dwMode;
		GetConsoleMode(hOutput, &dwMode);
		dwMode |= ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(hOutput, dwMode);
#endif
	}
	template <typename ...Args>
	void verbose(std::string func, unsigned int line, Args... args)
	{
		tm time;
#ifdef _WIN32
		localtime_s(&time, &time_now);
#else
		localtime_r(&time_now, &time);
#endif
		std::cout << "\u001b[35;1m[DEBUG]\u001b[38;5;245m" << " (" << func << ":" << line << ") " << std::put_time(&time, "%d-%m-%y %OH:%OM:%OS") << ": ";
		(std::cout << ... << args);
		std::cout << std::endl;
	}
	template <typename ...Args>
	void info(std::string func, unsigned int line, Args... args)
	{
		tm time;
#ifdef _WIN32
		localtime_s(&time, &time_now);
#else
		localtime_r(&time_now, &time);
#endif
		std::cout << "\u001b[34;1m[INFO]\033[0m" << " (" << func << ":" << line << ") " << std::put_time(&time, "%d-%m-%y %OH:%OM:%OS") << ": ";
		(std::cout << ... << args);
		std::cout << std::endl;
	}
	template <typename ...Args>
	void error(std::string func, unsigned int line, Args... args)
	{
		tm time;
#ifdef _WIN32
		localtime_s(&time, &time_now);
#else
		localtime_r(&time_now, &time);
#endif
		std::cout << "\x1B[31;1m[ERROR]\033[0m" << " (" << func << ":" << line << ") " << std::put_time(&time, "%d-%m-%y %OH:%OM:%OS") << ": ";
		(std::cout << ... << args);
		std::cout << std::endl;
	}
	template <typename ...Args>
	void warn(std::string func, unsigned int line, Args... args)
	{
		tm time;
#ifdef _WIN32
		localtime_s(&time, &time_now);
#else
		localtime_r(&time_now, &time);
#endif
		std::cout << "\u001b[33m[WARNING]\033[0m" << " (" << func << ":" << line << ") " << std::put_time(&time, "%d-%m-%y %OH:%OM:%OS") << ": ";
		(std::cout << ... << args);
		std::cout << std::endl;
	}
}