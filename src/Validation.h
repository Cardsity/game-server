#pragma once
#include <string>
#include <regex>

inline bool stringValidation(std::string text)
{
	static std::regex validation(R"rgx(^[a-zA-Z_\-0-9äüö ]{3,16}$)rgx");
	static std::smatch m;
	return std::regex_match(text, m, validation);
}
inline bool passwordValidation(std::string text)
{
	static std::regex validation(R"rgx(^[a-zA-Z_\-0-9äüö]{0,16}$)rgx");
	static std::smatch m;
	return std::regex_match(text, m, validation);
}
inline bool messageValidation(std::string text)
{
	static std::regex validation(R"rgx(^.{1,64}$)rgx");
	static std::smatch m;
	return std::regex_match(text, m, validation);
}
inline bool colorValidation(std::string color)
{
	static std::regex validation(R"rgx(^#[A-F0-9]{6}$)rgx");
	static std::smatch m;
	return std::regex_match(color, m, validation);
}