#pragma once
#include <type_traits>

#include <tuple>

#include <stdexcept>

namespace ulib
{
    /// @brief Appends values to a container.
    template <class T, typename = std::void_t<>>
    class container_appender
    {
    public:
        using value_type = typename T::value_type;

        container_appender(T &container) : _container(&container) {}

        value_type &append(const value_type &value) { return _container->emplace_back(value); }

    private:
        T *_container;
    };

    /// @brief A container_appender specialization for fixed-sized containers.
    template <class T>
    class container_appender<T, std::void_t<decltype(std::tuple_size<T>::value)>>
    {
    public:
        using value_type = typename T::value_type;

        container_appender(T &container) : _container(&container) {}

        value_type &append(const value_type &value)
        {
            if (index < std::tuple_size<T>::value)
                return (*_container)[index++] = value;
            else
                throw std::out_of_range("index overrun");
        }

    private:
        T *_container;
        size_t index = 0;
    };
} // namespace ulib
