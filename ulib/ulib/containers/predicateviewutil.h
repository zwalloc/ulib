#pragma once
#include <type_traits>

#include <ulib/typetraits/memfunc.h>
#include <ulib/typetraits/missingtype.h>
#include <ulib/typetraits/typeordefault.h>

namespace ulib
{
    template <template <class SpanT, class Pred, class... Args> class ViewT, class SelfT, class NewPred,
              class Check = std::enable_if_t<std::is_member_function_pointer_v<std::decay_t<NewPred>> || true>, class... NewArgs>
    auto CreatePredicateView(const SelfT &self, NewPred &&pred, NewArgs &&...args)
    {
        if constexpr (!std::is_member_function_pointer_v<std::decay_t<NewPred>>)
        {
            // printf("CreatePredicateView(const SelfT &self, NewPred &&pred, NewArgs &&...args)\n");
            return ViewT<SelfT, NewPred, NewArgs...>{self, std::forward<NewPred>(pred), std::forward<NewArgs>(args)...};
        }

        /*
                if constexpr (std::is_member_function_pointer_v<std::decay_t<NewPred>>)
                {
                    // printf("FN CreatePredicateView(const SelfT &self, NewPred &&pred, NewArgs &&...args)\n");
                    auto fn_pred = [pred, ... args = std::forward<NewArgs>(args)](mem_func_class_t<NewPred> instance) { return
           ((instance).*pred)(args...); }; return ViewT<SelfT, decltype(fn_pred)>{self, std::move(fn_pred)};
                }
                else
                {
                    // printf("CreatePredicateView(const SelfT &self, NewPred &&pred, NewArgs &&...args)\n");
                    return ViewT<SelfT, NewPred, NewArgs...>{self, std::forward<NewPred>(pred), std::forward<NewArgs>(args)...};
                }
        */
    }

    /*
        template <template <class SpanT, class Pred, class... Args> class ViewT, class SelfT, class... FuncArgs, class RetVal,
                  class VT = typename SelfT::value_type, class RetContValT = ulib::type_if_t<RetVal, !std::is_same_v<RetVal, void>>>
        auto CreatePredicateView(const SelfT &self, RetVal (VT::*fn)(FuncArgs &&...), FuncArgs &&...args)
        {
            printf("CreatePredicateView(const SelfT &self, RetVal (VT::*fn)(FuncArgs &&...), FuncArgs &&...args)\n");
            auto pred = [fn, ... args = std::forward<FuncArgs>(args)](VT &instance) { return ((instance).*fn)(args...); };
            return ViewT<SelfT, decltype(pred)>{self, std::move(pred)};
        }

        template <template <class SpanT, class Pred, class... Args> class ViewT, class SelfT, class... FuncArgs, class RetVal,
                  class VT = typename SelfT::value_type, class RetContValT = ulib::type_if_t<RetVal, !std::is_same_v<RetVal, void>>>
        auto CreatePredicateView(const SelfT &self, RetVal (VT::*fn)(FuncArgs &&...) const, FuncArgs &&...args)
        {
            printf("CreatePredicateView(const SelfT &self, RetVal (VT::*fn)(FuncArgs &&...) const, FuncArgs &&...args)\n");
            auto pred = [fn, ... args = std::forward<FuncArgs>(args)](const VT &instance) { return ((instance).*fn)(args...); };
            return ViewT<SelfT, decltype(pred)>{self, std::move(pred)};
        }
    */
} // namespace ulib
