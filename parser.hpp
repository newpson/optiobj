#pragma once
#include <stdexcept>
#include <optional>
#include "mesh.hpp"
#include <istream>

using std::runtime_error;
using std::string;
using std::istream;
using std::optional;

struct Triad
{
    // `pr/tr/nr`
    optional<int> position_ref;
    optional<int> texture_ref;
    optional<int> normal_ref;

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

struct ParsingError: public runtime_error
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
    static string message(const Type type)
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
        : runtime_error(message(type)), type(type), column(column), line(line)
    {}

    ParsingError(const ParsingError &error)
        : ParsingError(error.type, error.column, error.line)
    {}
};

Mesh load(istream &input);
Mesh load(const string &filename);