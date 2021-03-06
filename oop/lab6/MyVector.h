#ifndef MY_VECTOR_H
#define MY_VECTOR_H
#include<stdexcept>
template <class T>
class Vector {
public:
  Vector();                      // creates an empty vector
  Vector(int size);              // creates a vector for holding 'size' elements
  Vector(Vector<T>& r);       // the copy ctor
  ~Vector();                     // destructs the vector 
  T& operator[](int index);      // accesses the specified element without bounds checking
  T& at(int index);              // accesses the specified element, throws an exception of type 'std::out_of_range' when index <0 or >=m_nSize
  int size() const;         // return the size of the container
  void push_back(const T& x);    // adds an element to the end 
  void clear();                  // clears the contents
  bool empty() const;            // checks whether the container is empty 
private:
  void inflate();                // expand the storage of the container to a new capacity, e.g. 2*m_nCapacity
  T* m_pElements;                // pointer to the dynamically allocated storage
  int m_nSize;                   // the number of elements in the container
  int m_nCapacity;               // the number of elements that can be held in currently allocated storage
};
template <class T> T& Vector<T>::operator[](int index)
{
    return m_pElements[index];
}
template <class T> Vector<T>::Vector()
{
    m_nSize = 0;
    m_nCapacity = 2;//initial space(arbitrary chosen)
    m_pElements = new T [m_nCapacity];
}
template <class T> Vector<T>::Vector(int size)
{
    m_nSize = size;
    m_nCapacity = size + 1;
    m_pElements = new T [m_nCapacity];
}
template <class T> Vector<T>::Vector(Vector<T>& r)
{
    m_nCapacity = r.size() + 10;
    m_nSize = r.size();
    m_pElements = new T [m_nCapacity];
    for (int i = 0; i < m_nSize; i++) 
        m_pElements[i] = r[i];
}
template <class T> Vector<T>::~Vector()
{
    delete m_pElements;
}
template <class T> void Vector<T>::inflate()
{
    T* temp = new T [2 * m_nCapacity];//create new array
    for (int i = 0; i < m_nSize; i++) temp[i] = m_pElements[i];//copy the data to new array
    //update
    m_nCapacity *= 2;
    m_pElements = temp;
}
template <class T> void Vector<T>::push_back(const T& x)
{
    if (m_nSize >= m_nCapacity) inflate();
    m_pElements[m_nSize] = x;
    m_nSize++;
}
template <class T> T& Vector<T>::at(int index)
{
    if (index < 0 || index >= m_nSize) throw std::out_of_range("!");
    else return m_pElements[index];
}
template <class T> int Vector<T>::size() const
{
    return m_nSize;
}
template <class T> void Vector<T>::clear()
{
    m_nSize = 0;
    m_nCapacity = 2;
    delete m_pElements;
    m_pElements = new T [m_nCapacity];
}
template <class T> bool Vector<T>::empty() const
{
    if (m_nSize == 0) return true;
    else return false;
}
#endif
