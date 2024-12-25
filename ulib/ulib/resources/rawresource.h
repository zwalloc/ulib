#pragma once

#include "baseresource.h"

namespace ulib
{
	// class MainResource : public BaseResource {};
	template<class AllocatorT>
	using RawResource = BaseResource<AllocatorT>;
}