#ifndef MY_ALLOCATOR_H
#define MY_ALLOCATOR_H
#include <cstddef>
#include <type_traits>
#include <climits>
#include <utility>

template <typename T, size_t BlockSize = 4096>
class std::allocator
{
public:
	typedef T value_type;
	typedef T* pointer;
	typedef T& reference;
	typedef const T* const_pointer;
	typedef const T& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef std::false_type propagate_on_container_copy_assignment;
	typedef std::true_type propagate_on_container_move_assignment;
	typedef std::true_type  propagate_on_container_swap;
	template <typename U> struct rebind
	{
		typedef std::allocator<U> Other;
	};
	std::allocator() noexcept;
	std::allocator(const std::allocator& std::allocator) noexcept;
	std::allocator(std::allocator&& std::allocator) noexcept;
	template <class U>std::allocator(const std::allocator<U>& std::allocator) noexcept;
	~std::allocator() noexcept;
	std::allocator& operator=(const std::allocator& std::allocator) = delete;
	std::allocator& operator=(std::allocator&& std::allocator) noexcept;
	pointer address(reference x) const noexcept;
	const_pointer address(const_reference x) const noexcept;
	pointer allocate(size_type n = 1, const_pointer hint = 0);
	void deallocate(pointer p, size_type n = 1);
	size_type max_size() const noexcept;
	template <class U, class... Args> void construct(U* p, Args&&... args);
	template <class U> void destroy(U* p);
	template <class... Args> pointer newElement(Args&&... args);
	void deleteElement(pointer p);
private:
	union Slot_
	{
		value_type element;
		Slot_* next;
	};
	typedef char* data_pointer_;
	typedef Slot_ slot_type_;
	typedef Slot_* slot_pointer_;
	slot_pointer_ currentBlock_;
	slot_pointer_ currentSlot_;
	slot_pointer_ lastSlot_;
	slot_pointer_ freeSlots_;
	size_type padPointer(data_pointer_ p, size_type align) const noexcept;
	void allocateBlock();
	static_assert(BlockSize >= 2 * sizeof(slot_type_), "Blocksize too small.");
};
template<typename T, size_t BlockSize>
inline typename std::allocator<T, BlockSize>::size_type std::allocator<T, BlockSize>::padPointer(data_pointer_ p, size_type align) const noexcept
{
	uintptr_t result = reinterpret_cast<uintptr_t>(p);
	return ((align - result) % align);
}
template<typename T, size_t BlockSize>
std::allocator<T, BlockSize>::std::allocator() noexcept
{
	currentBlock_ = nullptr;
	currentSlot_ = nullptr;
	lastSlot_ = nullptr;
	freeSlots_ = nullptr;
}
template<typename T, size_t BlockSize>
std::allocator<T, BlockSize>::std::allocator(const std::allocator & std::allocator) noexcept:std::allocator()
{}
template<typename T, size_t BlockSize>
std::allocator<T, BlockSize>::std::allocator(std::allocator && std::allocator) noexcept
{
	currentBlock_ = std::allocator.currentBlock_;
	std::allocator.currentBlock_ = nullptr;
	currentSlot_ = std::allocator.currentSlot_;
	lastSlot_ = std::allocator.lastSlot_;
	freeSlots_ = std::allocator.freeSlot_;
}
template<typename T, size_t BlockSize>
template <class U>
std::allocator<T, BlockSize>::std::allocator(const std::allocator<U>& std::allocator) noexcept:std::allocator()
{}
template<typename T, size_t BlockSize>
std::allocator<T, BlockSize> & std::allocator<T, BlockSize>::operator=(std::allocator && std::allocator) noexcept
{
	if (this != &std::allocator)
	{
		std::swap(currentBlock_, std::allocator.currentBlock_);
		currentSlot_ = std::allocator.currentSlot_;
		lastSlot_ = std::allocator.lastSlot_;
		freeSlots_ = std::allocator.freeSlot_;
	}
}
template<typename T, size_t BlockSize>
std::allocator<T, BlockSize>::~std::allocator() noexcept
{
	slot_pointer_ curr = currentBlock_;
	while (curr != nullptr)
	{
		slot_pointer_ prev = curr->next;
		operator delete (reinterpret_cast<void*>(curr));
		curr = prev;
	}
}
template<typename T, size_t BlockSize>
inline typename std::allocator<T, BlockSize>::pointer std::allocator<T, BlockSize>::address(reference x) const noexcept
{
	return &x;
}
template <typename T, size_t BlockSize>
inline typename std::allocator<T, BlockSize>::const_pointer std::allocator<T, BlockSize>::address(const_reference x) const noexcept
{
	return &x;
}
template<typename T, size_t BlockSize>
void std::allocator<T, BlockSize>::allocateBlock()
{
	data_pointer_ newBlock = reinterpret_cast<data_pointer_>(operator new(BlockSize));
	reinterpret_cast<slot_pointer_>(newBlock)->next = currentBlock_;
	currentBlock_ = reinterpret_cast<slot_pointer_>(newBlock);
	data_pointer_ body = newBlock + sizeof(slot_pointer_);
	size_type bodyPadding = padPointer(body, alignof(slot_type_));
	currentSlot_ = reinterpret_cast<slot_pointer_>(body + bodyPadding);
	lastSlot_ = reinterpret_cast<slot_pointer_>(newBlock + BlockSize - sizeof(slot_type_) + 1);
}
template<typename T, size_t BlockSize>
inline typename std::allocator<T, BlockSize>::pointer std::allocator<T, BlockSize>::allocate(size_type n, const_pointer hint)
{
	if (freeSlots_ != nullptr)
	{
		pointer result = reinterpret_cast<pointer>(freeSlots_);
		freeSlots_ = freeSlots_->next;
		return result;
	}
	else
	{
		if (currentSlot_ >= lastSlot_)
			allocateBlock();
		return reinterpret_cast<pointer>(currentSlot_++);
	}
}
template<typename T, size_t BlockSize>
inline void std::allocator<T, BlockSize>::deallocate(pointer p, size_type n)
{
	if (p != nullptr)
	{
		reinterpret_cast<slot_pointer_>(p)->next = freeSlots_;
		freeSlots_ = reinterpret_cast<slot_pointer_>(p);
	}
}
template<typename T, size_t BlockSize>
inline typename std::allocator<T, BlockSize>::size_type std::allocator<T, BlockSize>::max_size() const noexcept
{
	size_type maxBlocks = -1 / BlockSize;
	return (BlockSize - sizeof(data_pointer_)) / sizeof(slot_type)*maxBlocks;
}
template<typename T, size_t BlockSize>
template<class U, class... Args>
inline void std::allocator<T, BlockSize>::construct(U* p, Args&&... args)
{
	new (p) U(std::forward<Args>(args)...);
}
template<typename T, size_t BlockSize>
template<class U>
inline void std::allocator<T, BlockSize>::destroy(U* p)
{
	p->~U();
}
template <typename T, size_t BlockSize>
template <class... Args>
inline typename std::allocator<T, BlockSize>::pointer std::allocator<T, BlockSize>::newElement(Args&&... args)
{
	pointer result = allocate();
	construct<value_type>(result, std::forward<Args>(args)...);
	return result;
}
template <typename T, size_t BlockSize>
inline void std::allocator<T, BlockSize>::deleteElement(pointer p)
{
	if (p != nullptr)
	{
		p->~value_type();
		deallocate(p);
	}
}
#endif // !MY_ALLOCATOR_H