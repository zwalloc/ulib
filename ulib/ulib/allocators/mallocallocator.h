#pragma once

#include <assert.h>
#include <stdlib.h>
#include <new>
#include <cstddef>

namespace ulib
{
	class MallocAllocator
	{
	public:
		struct Params
		{
		};

		MallocAllocator(Params params = {})
		{
		}

		MallocAllocator(MallocAllocator &&)
		{
		}

		MallocAllocator &operator=(MallocAllocator &&)
		{
			return *this;
		}

		static void *Alloc(size_t size)
		{
			void *ptr = ::malloc(size);
			if (!ptr)
				throw std::bad_alloc{};

			return ptr;
		}

		static void Free(void *page)
		{
			assert(page);
			::free(page);
		}
	};
}

inline void *operator new(size_t size, ulib::MallocAllocator &al) { return al.Alloc(size); }
inline void operator delete(void *ptr, ulib::MallocAllocator &al) { return al.Free(ptr); }
