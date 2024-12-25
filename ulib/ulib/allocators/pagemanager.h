#pragma once

// #undef NDEBUG
#include <assert.h>
#include <cstddef>

namespace ulib
{
    namespace pages
    {
        template <class T>
        struct BasicPage
        {
            T *freeNext;
            T *freePrev;
            T *pageNext;
            T *pagePrev;
            size_t size;
        };

        struct DefaultPage : BasicPage<DefaultPage>
        {
        };

        template <class ExtensionT>
        struct ExtendablePage : public BasicPage<ExtendablePage<ExtensionT>>
        {
            ExtensionT extension;
        };

        template <class PageT>
        struct BasicBlock
        {
            BasicBlock *next;
            BasicBlock *prev;
            size_t size;
            PageT firstPage;
        };

        namespace detail
        {
            template <class PageT>
            inline void *EndOfPage(PageT *page)
            {
                return (char *)page + page->size;
            }

            template <class PageT>
            inline void *PageContent(PageT *page)
            {
                return (char *)page + sizeof(PageT);
            }

            template <class PageT>
            inline PageT *SplitPage(PageT *page, size_t offset, size_t rightSize)
            {
                assert(offset >= sizeof(PageT));
                assert(offset <= page->size);
                assert(page->size >= offset);
                assert(page->size - offset >= sizeof(PageT));

                PageT *right = (PageT *)((char *)page + offset);
                right->size = rightSize;
                page->size = offset;

                right->pageNext = page->pageNext;
                right->pagePrev = page;

                if (right->pageNext)
                    right->pageNext->pagePrev = right;

                page->pageNext = right;

                return right;
            }

            template <class PageT>
            inline PageT *SplitPage(PageT *page, size_t offset)
            {
                assert(page->size >= offset);

                return SplitPage(page, offset, page->size - offset);
            }

            template <class PageT>
            inline void MergePage(PageT *left, PageT *right)
            {
                assert(left);
                assert(right);

                assert(left->pageNext == right);
                assert(right->pagePrev == left);

                left->size += right->size;

                if (right->pageNext)
                {
                    left->pageNext = right->pageNext;
                    right->pageNext->pagePrev = left;
                }
                else
                {
                    left->pageNext = nullptr;
                }
            }

            // from - inside node-free page
            // to - outside node-free page

            template <class PageT>
            inline void MovePageNodeFreePosition(PageT *from, PageT *to)
            {
                assert(from->freeNext);
                assert(from->freePrev);

                to->freeNext = from->freeNext;
                to->freePrev = from->freePrev;

                to->freeNext->freePrev = to;
                to->freePrev->freeNext = to;
            }

            template <class PageT>
            inline void InsertPageIntoTailOfRoundedNodeFree(PageT *node, PageT *page)
            {
                assert(node);
                assert(page);
                assert(node->freeNext);
                assert(node->freePrev);

                node->freePrev->freeNext = page;
                page->freeNext = node;
                page->freePrev = node->freePrev;
                node->freePrev = page;
            }

            template <class PageT>
            inline void DetachPageFromFreeNode(PageT *page)
            {
                page->freePrev->freeNext = page->freeNext;
                page->freeNext->freePrev = page->freePrev;
            }

            template <class PageT, size_t MinimumNewPageDataLength = 0x40>
            inline void AllocatePage(PageT *page, size_t allocationSize)
            {
                assert(page->size >= allocationSize);

                size_t rightSize = page->size - allocationSize;
                if (rightSize >= (sizeof(PageT) + MinimumNewPageDataLength))
                {
                    PageT *right = detail::SplitPage(page, allocationSize, rightSize);
                    detail::MovePageNodeFreePosition(page, right);
                }
                else
                {
                    detail::DetachPageFromFreeNode(page);
                }

                // freePrev nullptr useful for check page is free
                page->freePrev = nullptr;

                // freeNext must be undefined here
                // page->freeNext = nullptr;
            }

            template <class PageT, size_t MinimumNewPageDataLength = 0x40>
            inline bool AttemptAllocatePage(PageT *page, size_t allocationSize)
            {
                if (page->size >= allocationSize)
                {
                    detail::AllocatePage<PageT, MinimumNewPageDataLength>(page, allocationSize);
                    return true;
                }

                return false;
            }

            template <class PageT>
            inline bool IsPageFree(PageT *page)
            {
                return page->freePrev != nullptr;
            }
        }
    }

    template <class PageT = pages::DefaultPage, size_t MinimumNewPageDataLength = 0x40>
    class BasicPageManager
    {
    public:
        using Page = PageT;
        using Block = pages::BasicBlock<Page>;

        inline BasicPageManager()
        {
            mHeadBlock = nullptr;
            mNextFree = &mNodeFree;
            mNodeFree.size = 0;
            mNodeFree.pageNext = nullptr;
            mNodeFree.pagePrev = nullptr;
            mNodeFree.freeNext = &mNodeFree;
            mNodeFree.freePrev = &mNodeFree;
        }

        inline BasicPageManager(const BasicPageManager &) = delete;
        inline BasicPageManager(BasicPageManager &&) = delete;
        inline ~BasicPageManager() {}

        constexpr static size_t PageOverhead() { return sizeof(Page); }
        constexpr static size_t BlockOverhead() { return sizeof(Block) - sizeof(Page); }
        constexpr static size_t FreePageOverhead() { return sizeof(Block); }

        inline void AddFreePage(void *data, size_t size)
        {
            assert(size >= FreePageOverhead());

            Block *block = (Block *)data;
            block->size = size;

            RegisterMemoryBlockToList(block);

            Page *page = (Page *)&block->firstPage;
            page->size = block->size - BlockOverhead();
            page->pageNext = nullptr;
            page->pagePrev = nullptr;

            RegisterFreePageToList(page);

            if (!mNextFree)
                mNextFree = page;
        }

        inline size_t CalcFreePages()
        {
            size_t count = 0;
            Page *page = mNodeFree.freeNext;
            while (page != &mNodeFree)
            {
                count++;
                page = page->freeNext;
            }

            return count;
        }

        inline void AddFreePage(void *begin, void *end)
        {
            AddFreePage(begin, size_t(end) - size_t(begin));
        }

        inline void *Alloc(size_t size)
        {
            if (Page *page = AllocPage(size + sizeof(Page)))
                return pages::detail::PageContent(page);

            return nullptr;
        }

        inline Page *AllocPage(size_t realSize)
        {
            Page *page = mNextFree;
            if (Alloc(page, realSize))
            {
                return page;
            }
            else
            {
                Page *page = mNodeFree.freeNext;

                do
                {
                    if (Alloc(page, realSize))
                        return page;

                    page = page->freeNext;
                } while (page != &mNodeFree);
            }

            return nullptr;
        }

        inline void Free(void *ptr)
        {
            assert(ptr);
            FreePage((Page *)((char *)ptr - sizeof(Page)));
        }

        inline void FreePage(Page *page)
        {
            assert(page->freePrev == nullptr);

            Page *next = page->pageNext;
            Page *prev = page->pagePrev;

            if (prev && pages::detail::IsPageFree(prev))
            {
                pages::detail::MergePage(prev, page);

                if (next && pages::detail::IsPageFree(next))
                {
                    pages::detail::MergePage(prev, next);
                    pages::detail::DetachPageFromFreeNode(next);

                    if (next == mNextFree)
                        mNextFree = mNodeFree.freeNext;
                }
            }
            else if (next && pages::detail::IsPageFree(next))
            {
                pages::detail::MergePage(page, next);
                pages::detail::MovePageNodeFreePosition(next, page);

                if (next == mNextFree)
                    mNextFree = mNodeFree.freeNext;
            }
            else
            {
                pages::detail::InsertPageIntoTailOfRoundedNodeFree(&mNodeFree, page);
            }
        }

        inline Block *GetHeadBlock() { return mHeadBlock; }
        inline Page *GetNodeFree() { return &mNodeFree; }

    private:
        inline void RegisterMemoryBlockToList(Block *block)
        {
            block->prev = nullptr;
            block->next = mHeadBlock;

            mHeadBlock = block;
            if (Block *next = block->next)
                next->prev = block;
        }

        inline void RegisterFreePageToList(Page *page)
        {
            pages::detail::InsertPageIntoTailOfRoundedNodeFree(&mNodeFree, page);
        }

        inline bool Alloc(Page *page, size_t realSize)
        {
            assert(page);

            if (page->size >= realSize)
            {
                pages::detail::AllocatePage<Page, MinimumNewPageDataLength>(page, realSize);

                // also freeNext can be == &mNodeFree but that's a different story
                mNextFree = page->pageNext ? page->pageNext : mNodeFree.freeNext;

                assert(page->freePrev == nullptr);

                return true;
            }

            return false;
        }

        Block *mHeadBlock;
        Page mNodeFree;
        Page *mNextFree;
    };

    using PageManager = BasicPageManager<>;
}