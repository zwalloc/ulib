#pragma once


#include <new>

#ifdef _DEBUG
#include <assert.h>
#endif

namespace ulib
{
    struct DefaultHandledSingletonTag {};

    template <class T, class TagT = DefaultHandledSingletonTag>
    struct HandledSingleton
    {
        inline static T *RawData()
        {
            static char buf[sizeof(T)];
            return (T *)buf;
        }

#ifdef _DEBUG
    private:
        inline static bool &InitFlag()
        {
            static bool isInit = false;
            return isInit;
        }

    public:
        template <class... Args>
        inline static T &Construct(Args &&...args)
        {
            assert(!InitFlag());
            new (RawData()) T(args...);
            InitFlag() = true;
            return *RawData();
        }

        inline static T &Instance()
        {
            assert(InitFlag());
            return *RawData();
        }
#else
        template <class... Args>
        inline static T &Construct(Args &&...args)
        {
            new (RawData()) T(args...);
            return Instance();
        }

        inline static T &Instance() { return *RawData(); }
#endif
    };
}