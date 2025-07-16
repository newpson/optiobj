#include "utility.hpp"

#include <format>
#include <string>
#include <sstream>
#include <fstream>
#include <stdexcept>

using std::string;
using std::stringstream;
using std::ifstream;
using std::runtime_error;

string file_to_string(const string path)
{
    ifstream input(path, std::ios::in);
    if (!input.is_open())
        throw runtime_error(std::format("cannot read \"{}\"", path));
    stringstream content_stream;
    content_stream << input.rdbuf();
    const string content = content_stream.str();
    input.close();
    return content;
}
