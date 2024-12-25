#pragma once

#include <stdexcept>

namespace ulib
{
    struct UnsupportedEncodingException : public std::runtime_error
    {
        UnsupportedEncodingException()
            : std::runtime_error("unsupported encoding")
        {
        }
    };

    struct InvalidCodePointException : public std::runtime_error
    {
        InvalidCodePointException()
            : std::runtime_error("invalid codepoint")
        {
        }
    };

    struct CharacterOutOfRangeException : public std::out_of_range
    {
        CharacterOutOfRangeException()
            : std::out_of_range("character out of range")
        {
        }
    };
}
