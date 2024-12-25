#pragma once

#include <ulib/typetraits/field.h>

#define ULIB_RAW_MOVEABLE() using UlibRawMoveableT = std::bool_constant<true>;

namespace ulib
{
    ULIB_DEFINE_TYPE_FIELD_CHECK(explicit_raw_moveable_flag, UlibRawMoveableT);
}