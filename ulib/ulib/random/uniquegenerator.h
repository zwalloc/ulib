#pragma once

#include <ulib/containers/list.h>
#include <stdlib.h> // rand

namespace ulib
{
	template<class IntegerT, class AllocatorT = DefaultAllocator>
	class UniqueGenerator
	{
	public:
		inline UniqueGenerator(typename AllocatorT::Params params = {})
			: mValues(params)
		{

		}

		inline bool IsExist(IntegerT value)
		{
			for (IntegerT it : mValues)
			{
				if (it == value)
					return true;
			}

			return false;
		}

		inline IntegerT Generate()
		{
			while (true)
			{
				IntegerT value = Rand();
				if (value == 0)
					continue;

				if (IsExist(value))
					continue;

				mValues.Add(value);
				return value;
			}
		}

		inline void Remove(IntegerT value)
		{
			mValues.FastRemove(value);
		}

		inline size_t Size() { return mValues.Size(); }

	private:
		inline IntegerT Rand()
		{
			IntegerT value = 0;
			size_t base = sizeof(IntegerT) * 8;
			while (base)
			{
				base -= sizeof(short) * 8;
				value += IntegerT(::rand() << base);
			}

			return value;
		}

		ulib::List<IntegerT, AllocatorT> mValues;
	};
}