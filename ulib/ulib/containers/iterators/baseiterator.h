#pragma once

namespace ulib
{
	template<class T, class Category>
	struct BaseIterator
	{
		using iterator_category = Category;
		using value_type = T;
		using difference_type = size_t;
		using pointer = value_type*;
		using reference = value_type&;
	};
}