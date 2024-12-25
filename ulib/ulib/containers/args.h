#pragma once

#include <ulib/types.h>

namespace ulib
{
    namespace args
	{
		struct Capacity
		{
			Capacity(size_t c) : cc(c) {}
			size_t cc;
		};
	}
}