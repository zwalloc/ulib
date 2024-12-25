#pragma once
#include "predicateviewutil.h"

namespace ulib
{
    template <class SpanT, class Pred, class... Args>
    class MapView;

    template <class SelfT, class NewPred, class... NewArgs>
    auto CreateMapView(const SelfT &self, NewPred &&pred, NewArgs &&...args)
    {
        return CreatePredicateView<MapView, SelfT>(self, std::forward<NewPred>(pred), std::forward<NewArgs>(args)...);
    }
} // namespace ulib
