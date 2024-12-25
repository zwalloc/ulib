#pragma once

#include <ulib/types.h>
#include <assert.h>

namespace ulib
{
	template<class T = uint64>
	constexpr T Align(T value, T alignment)
	{
		assert(alignment != 0);

		return (value - 1) + (alignment - ((value - 1) % alignment));
	}
}