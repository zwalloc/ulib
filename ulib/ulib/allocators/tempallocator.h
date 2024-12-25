#pragma once

// #include <ulib/allocators/fastmemallocator.h>
// #include <ulib/allocators/staticallocator.h>
// #include <ulib/allocators/synchronizedallocator.h>
// #include <ulib/allocators/mallocallocator.h>

#include <ulib/allocators/defaultallocator.h>
#include <ulib/allocators/staticallocator.h>

namespace ulib
{
    struct TempAllocatorTagT {};
    // using TempAllocatorT = ulib::StaticAllocator<ulib::SynchronizedAllocator<ulib::FastMemAllocator<ulib::MallocAllocator>>, TempAllocatorTagT>;
    using TempAllocatorT = StaticAllocator<DefaultAllocator, TempAllocatorTagT>;
}