<div style="
            height:1000px;
            width:800px;
            text-align:center;
            display:table-cell;
            vertical-align:middle
    ">
    <center>
    	<h1>
            STL allocator+memory pool实验报告
        </h1>
        <h2 style="margin-top:100px">
            ***
        </h2>
        <h3 style="margin-top:20x">
            319010****
        </h3>
        <h4 style="margin-top:80px">
            Date:2021-06-23
        </h4>
    </center>
</div>

# STL allocator+memory pool实验报告

## 1 简介

在C++ STL中定义了许多容器，对容器进行内存分配需要使用allocator。memory pool为事先分配好的一大块内存，并有相应内存管理机制。

由于标准allocator频繁调用系统内存管理函数，将会造成性能损耗。本实验设计了一个基于memory

pool内存管理的allocator。allocator在被调用时通过memory pool进行内存分配，优先使用memory pool中的空间；当无法使用memory pool空间时，再调用系统内存管理函数。这样将大大减少系统内存管理的使用，提高运行性能。

## 2 设计实现

#### 2.1 struct Block

memory pool中的大块内存难以整体使用，因此将其切分为许多小的Block，每一小块独立存储，分配给不同任务。空闲的Block之间相互串联，形成free list，因此需要后继指针指向后续的Block。

```c++
struct Block
{
    unsigned char* Data;
    Block* Next;
};
```

#### 2.2 struct ExtraMem

由于Block为memory pool的一小部分，其存储空间有限，而且memory pool本身的空间也是有限的。因此当遇到无法存入Block的数据时，需要调用系统接口分配相应大小的内存。这些额外内存区仍需串联为一个链表，以便后续释放内存时将空间返回memory pool。

```c++
struct ExtraMem
{
    ExtraMem* Next;
};
```

#### 2.3 class MemoryPool

负责memory pool中内存空间的管理。FreeHead保存free list表头，ExHead保存额外内存区链表表头，Mem为具体内存区。

构造函数MemoryPool()负责表的初始化，将内存区串联为free list；~MemoryPool()为默认析构函数，在使用完毕时释放内存；allocate函数负责分配内存空间，当有空余block时从free list中取出block，当无空余block时申请新block空间，当数据无法写入block时申请额外内存空间；deallocate函数负责内存空间回收，将内存加入free list中供之后使用。

```c++
template <class T>
class MemoryPool
{
private:
    Block<T>* FreeHead;
    ExtraMem* ExHead;
    unsigned char* Mem = new unsigned char [BlockNum * sizeof(Block<T>)];
public:
    MemoryPool()
    ~MemoryPool()
    T* allocate(size_t n)
    void deallocate(T* p)
};
```

#### 2.4 class MyAllocator

allocator的实现，主要包括标准数据接口的和函数接口的定义。具体功能实现在class MemoryPool中，在实现功能时只需接受输入数据，然后调用class MemoryPool中的接口即可。

```c++
template <class T>
class MyAllocator
{
private:
    MemoryPool<T> Mem;
public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    template <typename anotherT> struct rebind { typedef MyAllocator<anotherT> other; };
    MyAllocator()
    MyAllocator(const MyAllocator& myallo)
    template<class anotherT> MyAllocator(const MyAllocator<anotherT>& other){}
    pointer allocate(size_type n)
    void deallocate(pointer p, size_type n)
    void construct(pointer p, const_reference val)
    pointer address(reference val)
    size_type max_size() const
    void destroy(pointer p)
};
```

## 3 测试结果

测试时间程序在提供的测试代码基础上，在操作前后添加计时函数实现的。不同规模的数据通过改变TESTSIZE的大小实现。

| 数据规模 | 运行时间（ms） |
| -------- | -------------- |
| 1000     | 435            |
| 2000     | 905            |
| 5000     | 2220           |
| 10000    | 4756           |
| 20000    | 8790           |

程序运行正常。通过测试时间结果可以发现，运行时间与输入规模大致呈线性关系。

## 4 总结

本实验实现了一个采用memory pool进行内存管理的allocator，可以减少内存管理的系统调用次数。理论上可以减少内存调用时间，提升运行性能。

在实验设计中我运用了c++类的设计思想，对STL中内容进行了改写，对面向对象的程序设计有了更深的理解。实验也锻炼了我的编程能力，巩固了对基础知识的掌握程度，在这次实验中我收获颇丰。

## 参考资料

[1] https://blog.csdn.net/captain_mxd/article/details/77487937

[2] https://www.cnblogs.com/wpcockroach/archive/2012/05/10/2493564.html

[3] https://en.cppreference.com/w/cpp/memory/allocator