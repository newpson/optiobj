#include "util.hpp"
#include <format>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

std::string file_to_string(const std::string path)
{
    std::ifstream input(path, std::ios::in);
    if (!input.is_open())
        throw std::runtime_error(std::format("cannot read \"{}\"", path));
    std::stringstream content_stream;
    content_stream << input.rdbuf();
    const std::string content = content_stream.str();
    input.close();
    return content;
}
