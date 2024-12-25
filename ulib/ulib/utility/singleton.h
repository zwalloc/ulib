#pragma once

namespace ulib
{
    struct DefaultHandledSingletonTag {};

    template <class T, class TagT = DefaultHandledSingletonTag>
    struct Singleton
    {
        template <class... Args>
        inline static T &Instance(Args &&...args)
        {
            static T o(args...);
            return o;
        }
    };
}