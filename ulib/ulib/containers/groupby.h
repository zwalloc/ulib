#pragma once
#include <ulib/allocators/defaultallocator.h>
#include <ulib/typetraits/container.h>

#include <type_traits>

#include <utility>

#include "appender.h"

namespace ulib
{
    template <class T, class A>
    class List;

    template <class GroupResultT = void, class ResultT = void, class ContT = void, class LambdaT = void,
              class ValueT = ulib::determined_value_type_or_die_t<ContT>, class GroupT = std::invoke_result_t<LambdaT, ValueT>,
              class AllocatorT = constainer_choose_ulib_allocator_t<ContT>>
    auto group_by(const ContT &cont, LambdaT &&pred)
    {
        using default_group_result_type = List<ValueT, AllocatorT>;
        using group_result_type = std::conditional_t<std::is_void_v<GroupResultT>, default_group_result_type, ResultT>;

        using group_pair_type = std::pair<GroupT, group_result_type>;

        using default_result_type = List<std::pair<GroupT, List<ValueT, AllocatorT>>, AllocatorT>;
        using result_type = std::conditional_t<std::is_void_v<ResultT>, default_result_type, ResultT>;

        using group_appender_type = container_appender<group_result_type>;
        using result_appender_type = container_appender<result_type>;

        result_type result;

        result_appender_type result_appender{result};

        auto find_package = [&](const GroupT &gr) -> group_pair_type * {
            for (auto &package : result)
            {
                if (package.first == gr)
                    return &package;
            }

            return nullptr;
        };

        auto get_package = [&](const GroupT &gr) -> group_pair_type & {
            if (group_pair_type *pPackage = find_package(gr))
                return *pPackage;

            group_pair_type pair;
            pair.first = gr;

            return result_appender.append(pair);
        };

        for (auto &info : cont)
        {
            auto &package = get_package(pred(info));

            group_appender_type group_appender{package.second};
            group_appender.append(info);
        }

        return result;
    }
} // namespace ulib
