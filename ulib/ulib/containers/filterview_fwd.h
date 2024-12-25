#pragma once
#include "predicateviewutil.h"

namespace ulib
{
    template <class SpanT, class Pred, class... Args>
    class FilterView;

    template <class SelfT, class NewPred, class... NewArgs>
    auto CreateFilterView(const SelfT &self, NewPred &&pred, NewArgs &&...args)
    {
        return CreatePredicateView<FilterView, SelfT>(self, std::forward<NewPred>(pred), std::forward<NewArgs>(args)...);
    }
} // namespace ulib
