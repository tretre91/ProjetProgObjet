#ifndef LAB_EXCEPTION_H
#define LAB_EXCEPTION_H

#include <fmt/core.h>
#include <stdexcept>

class ParseError : public std::exception
{
private:
	std::string _what;

public:
	ParseError(const char* error_msg) : _what(fmt::format("ERROR: Labyrinthe. {}", error_msg)) {}
	ParseError(const std::string& error_msg) : _what(fmt::format("ERROR: Labyrinthe. {}", error_msg)) {}

	const char* what() const noexcept override { return _what.c_str(); }
};

#endif
