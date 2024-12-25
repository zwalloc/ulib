#pragma once

namespace ulib
{
    // Primary template (will not be used directly)
    template <typename T>
    struct mem_func_class;

    // Specialization for member function pointers
    template <typename ReturnType, typename ClassType, typename... Args>
    struct mem_func_class<ReturnType (ClassType::*)(Args...)>
    {
        using type = ClassType;
    };

    // Specialization for const member function pointers
    template <typename ReturnType, typename ClassType, typename... Args>
    struct mem_func_class<ReturnType (ClassType::*)(Args...) const>
    {
        using type = const ClassType;
    };

    // Utility alias
    template <typename T>
    using mem_func_class_t = typename mem_func_class<T>::type;
} // namespace ulib
