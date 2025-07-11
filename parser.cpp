#include "parser.hpp"
#include <fstream>
#include <cctype> // FIXME use `<locale>`
#include <charconv>
#include <optional>

#include <iostream>

using std::ifstream;
using std::string;
using std::isspace;
using citer = string::const_iterator;

// TODO maybe remove `inline`, check on godbolt ================================
inline void skip_space(citer &iter, const citer &end)
{
    while (iter < end && isspace(*iter))
        ++iter;
}

inline bool is_endspace(citer &iter, const citer &end)
{
    return (iter >= end || isspace(*iter));
}

inline bool is_next_endspace(citer &iter, const citer &end)
{
    return is_endspace(++iter, end);
}

inline void skip_nonspace(citer &iter, const citer &end)
{
    while (!is_endspace(iter, end))
        ++iter;
}

inline bool is_delim(citer &iter, const citer &end)
{
    return (*iter == '/');
}

inline void skip_nondelim(citer &iter, const citer &end)
{
    while (!is_endspace(iter, end) && !is_delim(iter, end))
        ++iter;
}
// =============================================================================

LineType parse_linetype(citer &iter, const citer &end)
{
    skip_space(iter, end);

    if (iter >= end)
        return LineType::EMPTY;

    if (*iter == '#')
        return LineType::COMMENT;

    if (*iter == 'v') {
        if (is_next_endspace(iter, end))
            return LineType::POSITION;

        if (*iter == 't' && is_next_endspace(iter, end))
            return LineType::TEXTURE;

        if (*iter == 'n' && is_next_endspace(iter, end))
            return LineType::NORMAL;
    }

    if (*iter == 'f' && is_next_endspace(iter, end))
        return LineType::FACE;
    
    if (*iter == 'g' && is_next_endspace(iter, end))
        return LineType::GROUP;

    return LineType::UNKNOWN;
}

double parse_double(citer &iter, const citer &end)
{
    skip_space(iter, end);
    if (is_endspace(iter, end))
        throw ParsingError(ParsingError::Type::EXPECTED_FLOAT, end - iter);

    const citer content_begin = iter;
    skip_nonspace(iter, end);
    double parsed_value;
    const auto result = std::from_chars(content_begin.base(), iter.base(), parsed_value);
    if (result.ec == std::errc::invalid_argument || result.ec == std::errc::result_out_of_range)
        throw ParsingError(ParsingError::Type::EXPECTED_FLOAT, end - content_begin);

    return parsed_value;
}

int parse_index(citer &iter, const citer &end)
{
    const citer content_begin = iter;
    skip_nondelim(iter, end);
    int parsed_value;
    const auto result = std::from_chars(content_begin.base(), iter.base(), parsed_value);
    if (result.ec == std::errc::invalid_argument || result.ec == std::errc::result_out_of_range)
        throw ParsingError(ParsingError::Type::EXPECTED_INTEGER, end - content_begin);
    return parsed_value;
}

vec3 parse_vec3(citer &iter, const citer &end)
{
    vec3 position;
    for (int i = 0; i < 3; ++i)
        position[i] = parse_double(iter, end);
    return position;
}

vec2 parse_vec2(citer &iter, const citer &end)
{
    vec2 texture;
    for (int i = 0; i < 2; ++i)
        texture[i] = parse_double(iter, end);
    return texture;
}

Triad parse_triad(citer &iter, const citer &end)
{
    skip_space(iter, end);
    Triad triad;
    if (is_endspace(iter, end))
        return triad;
    triad.position_ref = parse_index(iter, end);
    // `pr`
    if (is_endspace(iter, end))
        return triad;
    ++iter;

    if (!is_delim(iter, end))
        triad.texture_ref = parse_index(iter, end);
    // `pr/tr`
    if (is_endspace(iter, end))
        return triad;
    ++iter;

    // `pr//`
    if (is_endspace(iter, end))
        throw ParsingError(ParsingError::Type::EXPECTED_INTEGER, end - iter);
    
    // `pr/[tr]/nr`
    triad.normal_ref = parse_index(iter, end);
    return triad;
}

bool Triad::is_coherent_to(const Triad &another) const
{
    if (position_ref == std::nullopt && another.position_ref != std::nullopt
        || position_ref != std::nullopt && another.position_ref == std::nullopt)
        return false;
    
    if (texture_ref == std::nullopt && another.texture_ref != std::nullopt
        || texture_ref != std::nullopt && another.texture_ref == std::nullopt)
        return false;
    
    if (normal_ref == std::nullopt && another.normal_ref != std::nullopt
        || normal_ref != std::nullopt && another.normal_ref == std::nullopt)
        return false;
    
    return true;
}

bool Triad::is_empty() const
{
    return !(position_ref.has_value() || texture_ref.has_value() || normal_ref.has_value()); 
}


vector<Triad> parse_triads(citer &iter, const citer &end)
{
    vector<Triad> triads;
    triads.reserve(3);

    triads.push_back(parse_triad(iter, end));
    while (iter < end) {
        Triad triad = parse_triad(iter, end);
        if (triad.is_empty())
            break;
        if (triads[0].is_coherent_to(triad))
            triads.push_back(triad);
        else
            throw ParsingError(ParsingError::Type::COMPONENTS_INCOHERENCE);
    }
    
    if (triads.size() < 3)
        throw ParsingError(ParsingError::Type::EXPECTED_INTEGER);

    return triads;
}

string parse_group_name(citer &iter, const citer &end)
{
    skip_space(iter, end);
    if (is_endspace(iter, end))
        throw ParsingError(ParsingError::Type::EXPECTED_STRING);
    const citer name_begin = iter;
    skip_nonspace(iter, end);
    // discard all group names but first
    return string(name_begin, iter);
}

Mesh load(istream &input)
{
    vector<vec3> positions;
    vector<vec2> textures;
    vector<vec3> normals;
    
    vector<int> positions_refs;
    vector<int> textures_refs;
    vector<int> normals_refs;
    vector<int> refs_ends = {0};

    vector<int> groups_ends = {0};
    vector<string> groups_names = {"default"};
    
    int line_number = 0;
    while (!input.eof()) {
        string line_buffer;
        std::getline(input, line_buffer);
        ++line_number;
        if (line_buffer.empty())
            continue;

        // TODO wrap to function
        while (*(line_buffer.end() - 1) == '\\') {
            *(line_buffer.end() - 1) = ' ';
            string line_buffer_next;
            std::getline(input, line_buffer_next);
            line_buffer += line_buffer_next;
            ++line_number;
        }

        auto line_iter = line_buffer.cbegin();
        auto line_end = line_buffer.cend();

        switch (parse_linetype(line_iter, line_end)) {
            case LineType::EMPTY:
                [[fallthrough]];
            case LineType::COMMENT:
                continue;
            case LineType::POSITION:
                positions.push_back(parse_vec3(line_iter, line_end));
                break;
            case LineType::TEXTURE:
                textures.push_back(parse_vec2(line_iter, line_end));
                break;
            case LineType::NORMAL:
                normals.push_back(parse_vec3(line_iter, line_end));
                break;
            case LineType::FACE:
                try {
                    const auto triads = parse_triads(line_iter, line_end);
                    for (const Triad &triad: triads) {
                        positions_refs.push_back(triad.position_ref.value());
                        if (triad.texture_ref != std::nullopt) // FIXME bullshit comparison #1
                            textures_refs.push_back(triad.texture_ref.value());
                        if (triad.normal_ref != std::nullopt) // FIXME bullshit comparison #2
                            normals_refs.push_back(triad.normal_ref.value());
                    }
                    refs_ends.push_back(positions_refs.size());
                } catch (const ParsingError &error) {
                    throw ParsingError(error.type, line_buffer.size() - error.column, line_number);
                }
                break;
            case LineType::GROUP:
                groups_names.push_back(parse_group_name(line_iter, line_end));
                groups_ends.push_back(refs_ends.size());
                break;
            case LineType::UNKNOWN:
                break;
        }
    }
    groups_ends.push_back(refs_ends.size());

    return Mesh(positions, textures, normals,
                positions_refs, textures_refs, normals_refs,
                refs_ends, groups_ends, groups_names);
}

Mesh load(const string &filename)
{
    ifstream input(filename, std::ios::in);
    if (!input.is_open())
        throw ParsingError(ParsingError::Type::INPUT);

    return load(input);
}