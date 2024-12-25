#pragma once

namespace ulib
{
    enum class ContainerTag
    {
        None,
        String,
        List,
        Map
    };

    // ContainerTypeT
    struct string_type_tag {};
    struct list_type_tag {};
    struct map_type_tag {};

    // ContainerOwnershipT
    struct view_ownership_tag {};
    struct store_ownership_tag {};

} // namespace ulib