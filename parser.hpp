#pragma once
#include <istream>
#include <optional>
#include <stdexcept>
#include "mesh.hpp"

struct Triad
{
    // `pr/tr/nr`
    std::optional<int> position_ref;
    std::optional<int> texture_ref;
    std::optional<int> normal_ref;

    bool is_coherent_to(const Triad &triad) const;
    bool is_empty() const;
};

enum class LineType
{
    EMPTY,
    COMMENT,
    POSITION,
    TEXTURE,
    NORMAL,
    FACE,
    GROUP,
    UNKNOWN,
};

struct ParsingError: public std::runtime_error
{
    enum class Type
    {
        EXPECTED_FLOAT,
        EXPECTED_INTEGER,
        EXPECTED_STRING,
        COMPONENTS_INCOHERENCE,
        UNDEFINED_INDEX,
        INPUT,
        INPUT_EMPTY,
        STATE_BROKEN,
    };

    // FIXME bullshit string conversion
    static std::string message(const Type type)
    {
        switch (type) {
        case Type::EXPECTED_FLOAT: return "float expected";
        case Type::EXPECTED_INTEGER: return "integer expected";
        case Type::EXPECTED_STRING: return "string expected";
        case Type::COMPONENTS_INCOHERENCE: return "incoherence detected";
        case Type::UNDEFINED_INDEX: return "undefined index";
        case Type::INPUT: return "invalid input";
        case Type::INPUT_EMPTY: return "input is empty";
        case Type::STATE_BROKEN: return "state is broken (fatal)";
        }
        return "unknown state (fatal)";
    }

    const int column;
    const int line;
    const Type type;

    ParsingError(const Type type, const int column = -1, const int line = -1)
        : runtime_error(message(type)), column(column), line(line), type(type)
    {}

    ParsingError(const ParsingError &error)
        : ParsingError(error.type, error.column, error.line)
    {}
};

Mesh load(std::istream &input);
Mesh load(const std::string &filename);
