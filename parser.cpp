#include "parser.hpp"
#include <cctype> // FIXME use `<locale>`
#include <charconv>
#include <cstddef>
#include <fstream>
#include <istream>
#include <optional>

using citer = std::string::const_iterator;

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

inline bool is_delim(citer &iter)
{
    return (*iter == '/');
}

inline void skip_nondelim(citer &iter, const citer &end)
{
    while (!is_endspace(iter, end) && !is_delim(iter))
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
    return parsed_value - 1; // TODO negative indices support
}

glm::vec3 parse_vec3(citer &iter, const citer &end)
{
    glm::vec3 position;
    for (int i = 0; i < 3; ++i)
        position[i] = parse_double(iter, end);
    return position;
}

glm::vec2 parse_vec2(citer &iter, const citer &end)
{
    glm::vec2 texture;
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

    if (!is_delim(iter))
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
    if ((position_ref == std::nullopt && another.position_ref != std::nullopt)
        || (position_ref != std::nullopt && another.position_ref == std::nullopt))
        return false;

    if ((texture_ref == std::nullopt && another.texture_ref != std::nullopt)
        || (texture_ref != std::nullopt && another.texture_ref == std::nullopt))
        return false;

    if ((normal_ref == std::nullopt && another.normal_ref != std::nullopt)
        || (normal_ref != std::nullopt && another.normal_ref == std::nullopt))
        return false;

    return true;
}

bool Triad::is_empty() const
{
    return !(position_ref.has_value() || texture_ref.has_value() || normal_ref.has_value());
}

std::vector<Triad> parse_triads(citer &iter, const citer &end)
{
    std::vector<Triad> triads;
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

std::string parse_group_name(citer &iter, const citer &end)
{
    skip_space(iter, end);
    if (is_endspace(iter, end))
        throw ParsingError(ParsingError::Type::EXPECTED_STRING);
    const citer name_begin = iter;
    skip_nonspace(iter, end);
    // discard all group names except first
    return std::string(name_begin, iter);
}

glm::vec3 generate_normal(const std::vector<Triad> &face, const std::vector<glm::vec3> &positions)
{
    // generate as average of normals of all triangles in polygon
    glm::vec3 normals_sum;
    for (std::size_t i = 1; i < face.size() - 1; ++i) {
        const glm::vec3 a(positions[face[i].position_ref.value()]
                          - positions[face[0].position_ref.value()]);
        const glm::vec3 b(positions[face[i+1].position_ref.value()]
                          - positions[face[0].position_ref.value()]);
        normals_sum += glm::cross(a, b);
    }
    return glm::normalize(normals_sum);
}

Mesh load(std::istream &input)
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> textures = { glm::vec2(0, 0) };
    constexpr size_t TEXTURE_DEFAULT_ID = 0;
    std::vector<glm::vec3> normals;

    std::vector<int> positions_refs;
    std::vector<int> textures_refs;
    std::vector<int> normals_refs;
    std::vector<int> refs_ends = {0};

    std::vector<int> groups_ends = {0};
    std::vector<std::string> groups_names = {"default"};

    int line_number = 0;
    while (!input.eof()) {
        std::string line_buffer;
        std::getline(input, line_buffer);
        ++line_number;
        if (line_buffer.empty())
            continue;

        // TODO wrap to function
        while (*(line_buffer.end() - 1) == '\\') {
            *(line_buffer.end() - 1) = ' ';
            std::string line_buffer_next;
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
                    int generated_normal_ref = -1;
                    if (triads[0].normal_ref == std::nullopt) {
                        normals.push_back(generate_normal(triads, positions));
                        generated_normal_ref = normals.size() - 1;
                    }
                    for (const Triad &triad: triads) {
                        positions_refs.push_back(triad.position_ref.value());
                        // FIXME kinda stupid checks inside loop
                        textures_refs.push_back(triad.texture_ref == std::nullopt ? TEXTURE_DEFAULT_ID : triad.texture_ref.value());
                        normals_refs.push_back(triad.normal_ref == std::nullopt ? generated_normal_ref : triad.normal_ref.value());
                    }
                    refs_ends.push_back(positions_refs.size());
                } catch (const ParsingError &error) {
                    throw ParsingError(error.type, line_buffer.size() - error.column, line_number);
                }
                break;
            case LineType::GROUP:
                groups_names.push_back(parse_group_name(line_iter, line_end));
                groups_ends.push_back(refs_ends.size() - 1);
                break;
            case LineType::UNKNOWN:
                break;
        }
    }
    groups_ends.push_back(refs_ends.size() - 1);

    return Mesh(positions, textures, normals,
                positions_refs, textures_refs, normals_refs,
                refs_ends, groups_ends, groups_names);
}

Mesh load(const std::string &filename)
{
    std::ifstream input(filename, std::ios::in);
    if (!input.is_open())
        throw ParsingError(ParsingError::Type::INPUT);

    return load(input);
}
