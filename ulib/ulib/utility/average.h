#pragma once

#include <ulib/types.h>

namespace ulib
{
	template<class T>
	class Average
	{
	public:
		inline Average()
			: mValue(0), mCount(0)
		{}

		inline Average(T value)
			: mValue(value), mCount(1)
		{}

		inline void Add(T value)
		{
			if (mCount == 0)
				mValue = value;
			else
			{
				if(value > mValue)
					mValue += (value - mValue) / mCount;
				else
					mValue -= (mValue - value) / mCount;
			}

			mCount++;
		}

		inline T Value() const
		{
			return mValue;
		}

		inline uint64 Count() const
		{
			return mCount;
		}

	private:
		T mValue;
		uint64 mCount;
	};
}