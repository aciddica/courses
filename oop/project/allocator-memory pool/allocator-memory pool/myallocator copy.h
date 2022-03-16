#ifndef MY_ALLOCATOR_H
#define MY_ALLOCATOR_H

#include<new> //placement new
#include<cstddef> //ptrdiff_t size_t
#include<iostream>
//�����ڴ�

#define BUFFER_SIZE 1024
using namespace std;

template <typename T>
struct Block {
	Block *next;
};

template <typename T>
class Buffer
{
private:
	//blocksize����Ҫ��װ��Block��Ϣ
	static const size_t blockSize = sizeof(T);
	unsigned char* data = new unsigned char[blockSize * BUFFER_SIZE];//����һ��buffer�Ŀռ�
	Buffer * next;

public:
	Buffer() = default;//���캯��
	Buffer * getNext() const{return next;}
	void setNext(Buffer *p) { next = p; }
	T *getBlock(size_t index)	{//��ȡindex��block��Ӧָ��
		return reinterpret_cast<T *>(data + blockSize * index);
	}
};

template <typename T>
class MemoryPool
{
private:
	Buffer<T> *HeadBuffer = new Buffer<T>;//ê��
	Buffer<T> *curBuffer = NULL;
	size_t bufferedBlocks = BUFFER_SIZE;
	Block<T> * firstFreeBlock;//�������ɵ��ڴ�
	Block<T> * bigMem;//��������Buffer��С���ڴ�

public:
	MemoryPool(){
		HeadBuffer->setNext(NULL);
		firstFreeBlock = NULL;
	}
	//��֧���޲������캯��
	MemoryPool(MemoryPool &&memoryPool) = delete;
	MemoryPool(const MemoryPool &memoryPool) = delete;
	MemoryPool operator =(MemoryPool &&memoryPool) = delete;
	MemoryPool operator =(const MemoryPool &memoryPool) = delete;

	~MemoryPool()
	{
		//�ͷ�����Buffer
		while (HeadBuffer) {
			Buffer<T> *buffer = HeadBuffer;
			HeadBuffer = buffer->getNext();
			delete buffer;
		}
		//�ͷų���Buffer��С�Ĵ��ڴ�
		while (bigMem)
		{
			T* temp = reinterpret_cast<T *>(bigMem);
			bigMem = bigMem->next;
			::operator delete(temp);
		}
	}

	T *allocate(size_t n)
	{
		if (n == 1)
		{
			if (firstFreeBlock) {
				Block<T> *block = firstFreeBlock;
				firstFreeBlock = block->next;
				return reinterpret_cast<T *>(block);
			}

			if (bufferedBlocks + n >= BUFFER_SIZE) {//�������1������������BUFFER_SIZE��������bufferedBlocks��0��ʼ����������
				Buffer<T>*p = new Buffer<T>;
				p->setNext(HeadBuffer->getNext());
				HeadBuffer->setNext(p);
				curBuffer = p;
				bufferedBlocks = 0;
			}

			return curBuffer->getBlock(bufferedBlocks++);
		}	
		else
		{
			//��n��Buffer��С������ôʹ���ڴ�������ǲ�����ģ����ڴ�ʹ��new/delete��Ϊ����ɿ�
			if (n > BUFFER_SIZE)
			{
				T* temp = (T*)::operator new (sizeof(T)*n);
				Block<T> * block = reinterpret_cast<Block<T> *>(temp);
				block->next = bigMem;
				bigMem = block;
				return temp;
			}
			
			if (bufferedBlocks + n >= BUFFER_SIZE) {//�������n������������BUFFER_SIZE��������bufferedBlocks��0��ʼ����������
				Buffer<T>*p = new Buffer<T>;
				p->setNext(HeadBuffer->getNext());
				HeadBuffer->setNext(p);
				//��buffer�в���n��ʣ���ҵ�freeBlock��
				int i = bufferedBlocks;
				while (i < BUFFER_SIZE)
				{
					Block<T> * temp = reinterpret_cast<Block<T> *>(curBuffer->getBlock(i));
					temp->next = firstFreeBlock;
					firstFreeBlock = temp;
					i++;
				}
				//����Buffer
				curBuffer = p;
				bufferedBlocks = 0;
			}

			bufferedBlocks += n;
			return curBuffer->getBlock(bufferedBlocks-n);
		}
	}

	void deallocate(T *pointer)
	{
		//���黹���ڴ�ֱ�ӹҵ�freeBlock�ϣ�����δ֪��С����������һ��block����ڴ���Ƭ		
		Block<T> *block = reinterpret_cast<Block<T> *>(pointer);
		block->next = firstFreeBlock;
		firstFreeBlock = block;
		//�����´洢��Ϣ�������Ҳ����Ϊ�ڴ汻ʹ�ö������ǣ�
		//ͬʱ���ٿռ�洢��ϢҲ���˷ѿռ䣬�����ڶദ������Ҳ�ή���ٶȣ�
	}
};

template<typename T>
class MyAllocator {
private:
	//�ֿ�����MyAllocate��copyMyAllocate��rebindMyAllocate
	MyAllocator *copyMyAllocate = NULL;
	allocator<T> *rebindMyAllocate = NULL;
	MemoryPool<T> MP;//�����ڴ��
public:
	//�ͱ���
	typedef T value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	template<class U>
	struct rebind {
		typedef MyAllocator<U> other;
	};

	MyAllocator() = default;
	MyAllocator(MyAllocator &allocator) :	copyMyAllocate(&allocator){}
	template <class U>
	MyAllocator(const MyAllocator<U> &other)
	{
		if (!std::is_same<T, U>::value)
			rebindMyAllocate = new allocator<T>();
	}
	~MyAllocator(){
		delete copyMyAllocate;
		delete rebindMyAllocate;
	}

	pointer allocate(size_type n, const void *hint = 0)
	{
		if (copyMyAllocate)
			return copyMyAllocate->allocate(n, hint);
		if (rebindMyAllocate)
			return rebindMyAllocate->allocate(n, hint);
		/*
			if (n != 1 || hint)
			throw std::bad_alloc();		*/
		//���޸�
		T* temp =  MP.allocate(n);
		//T* temp = (T*)(::operator new((size_t)(sizeof(T))));
		return temp;
	}

	void deallocate(pointer p, size_type n)
	{
		if (copyMyAllocate) {
			copyMyAllocate->deallocate(p, n);
			return;
		}
		if (rebindMyAllocate) {
			rebindMyAllocate->deallocate(p, n);
			return;
		}
		MP.deallocate(p);
		//::operator delete(p);//���޸�
	}

	void construct(pointer ptr, value_type value) {
		new (ptr) T(value); //��λnew����ָ����ַ���������
	}
	void destroy(pointer ptr) {
		ptr->~T();
	}
	pointer address(reference x) {
		return (pointer)&x;
	}
	const_pointer address(reference x) const {
		return (const_pointer)&x;
	}
	size_type max_size() const {//���ڳ���Buffer�Ĵ��ڴ�ʹ��new/delete���ɷ����ڴ��С����ȡ�����ڴ��С�������UINT_MAX�й�
		return size_type(UINT_MAX / sizeof(T));
	}
	void newElement(value_type value)
	{
		pointer result = allocate();
		construct<value_type>(result, value);
		return result;
	}
	void deleteElement(pointer p)
	{
		if (p != nullptr)
		{
			p->~value_type();
			deallocate(p);
		}
	}
};

#endif