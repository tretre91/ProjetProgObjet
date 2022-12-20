#ifndef LAB_EXCEPTION_H
#define LAB_EXCEPTION_H

#include <stdexcept>
#include <fmt/core.h>

class ParseError : public std::exception
{
private:
    std::string m_what;

public:
    ParseError(const char* error_msg) : m_what(fmt::format("ERROR: Labyrinthe. {}", error_msg)) {}
    ParseError(const std::string& error_msg) : m_what(fmt::format("ERROR: Labyrinthe. {}", error_msg)) {}

    const char* what() const noexcept override { return m_what.c_str(); }
};

#endif
