/**
 * @file Exception.h
 * This file contains the custom exceptions used in the project.
 */

#ifndef LAB_EXCEPTION_H
#define LAB_EXCEPTION_H

#include <fmt/core.h>
#include <stdexcept>

/**
 * @brief Exception thrown when parsing a labyrinth file.
 */
class ParseError : public std::exception
{
private:
	std::string _what; // the error message

public:
	ParseError(const char* error_msg) : _what(fmt::format("ERROR: Labyrinthe. {}", error_msg)) {}
	ParseError(const std::string& error_msg) : _what(fmt::format("ERROR: Labyrinthe. {}", error_msg)) {}

	/**
	 * @brief Returns this exception's error message.
	 */
	const char* what() const noexcept override { return _what.c_str(); }
};

#endif
