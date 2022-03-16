#ifndef MY_ALLOCATOR_H
#define MY_ALLOCATOR_H

#include<iostream>
#include<cstddef>
#include<stdlib.h>
using namespace std;
#define BlockSize 256
#define BlockNum 1024
template <class T>
struct Block//block of the memory pool
{
    unsigned char* Data = new unsigned char [BlockSize * sizeof(T)];
    Block* Next;
};
struct ExtraMem//extra memory space to store data can't be obtained in the block
{
    ExtraMem* Next;
};
template <class T>
class MemoryPool
{
private:
    Block<T>* FreeHead;//head of the free block list
    ExtraMem* ExHead;//head of the extra memory list
    unsigned char* Mem = new unsigned char [BlockNum * sizeof(Block<T>)];
public:
    MemoryPool()//constructor
    {
        FreeHead = (Block<T>*)Mem;
        FreeHead->Next = NULL;
        ExHead = NULL;
        int pos = sizeof(Block<T>);
        while(pos < BlockNum * sizeof(Block<T>)) //initialize the free list
        {
            Block<T>* temp = (Block<T>*) (Mem + pos);
            temp->Next = NULL;
            FreeHead->Next = temp;
             FreeHead = temp;
            pos += sizeof(Block<T>);
        }
    }
    ~MemoryPool(){}

    T* allocate(size_t n)//allocate space using memory pool
    {
        if(n <= BlockSize)//if the space required can be obtained in a block
        {
            if (FreeHead != NULL)//get block from the free list
            {
                Block<T>* allo = FreeHead;
                FreeHead = FreeHead->Next;
                return reinterpret_cast<T*>(allo);
            }
            else//create a new block
            {
                Block<T>* allo = new Block<T>;
                allo->Next = FreeHead;
                return reinterpret_cast<T*>(allo);

            }
        }
        else//create a large memory space
        {
            ExtraMem* allo = (ExtraMem*)malloc(sizeof(T) * n + sizeof(ExtraMem*));
            allo->Next = ExHead;
            ExHead = allo;
            return reinterpret_cast<T*>(allo);
        }
    }
    void deallocate(T* p)//deallocate by adding the pointer to the freelist's head
    {
        Block<T>* freeBlock = reinterpret_cast<Block<T>*>(p);
        freeBlock->Next = FreeHead;
        FreeHead = freeBlock;
    }
};
template <class T>
class MyAllocator
{
private:
    MemoryPool<T> Mem;
public:
    //value types
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    template <typename anotherT> struct rebind { typedef MyAllocator<anotherT> other; };
    MyAllocator() {}
    MyAllocator(const MyAllocator& myallo) {}
    template<class anotherT> MyAllocator(const MyAllocator<anotherT>& other){}
    pointer allocate(size_type n)//call the memory pool allocate
    {
        T* temp = Mem.allocate(n);
        return temp;
    }
    void deallocate(pointer p, size_type n)//call the memory pool deallocate
    {
        Mem.deallocate(p);
        return ;
    }
    void construct(pointer p, const_reference val)
    {

        p = allocate(sizeof(value_type));
        *p = val;
    }
    pointer address(reference val)
    {
        return reinterpret_cast<pointer>(&val);
    }
    size_type max_size() const
    {
        return (UINT_MAX / sizeof(T));
    }
    void destroy(pointer p)
    {
        deallocate(p, sizeof(Block<T>));
    }

};

#endif