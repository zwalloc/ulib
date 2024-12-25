#pragma once

#ifdef _DEBUG
#ifdef ULIB_USE_DEBUG_RESOURCE
#define ULIB_FORCE_DEBUG_RESOURCE_USE
#endif
#endif

#ifdef ULIB_FORCE_DEBUG_RESOURCE_USE
#include "debugresource.h"

namespace ulib
{
	template<class AllocatorT>
	using Resource = DebugResource<AllocatorT>;
}

#else
#include "rawresource.h"

namespace ulib
{
	template<class AllocatorT>
	using Resource = RawResource<AllocatorT>;
}

#endif