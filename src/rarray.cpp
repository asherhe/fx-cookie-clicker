#include "rarray.h"
#include <stddef.h>
#include <string.h>
#include <gint/kmalloc.h>

template <typename T>
typename rarray<T>::ptr &rarray<T>::ptr::operator=(const rarray<T>::ptr &p)
{
  rb_ = p.rb_;
  idx_ = p.idx_;
  return *this;
}

template <typename T>
typename rarray<T>::ptr &rarray<T>::ptr::operator++()
{
  ++idx_;
  check_bounds();
  return *this;
}
template <typename T>
typename rarray<T>::ptr rarray<T>::ptr::operator++(int)
{
  ptr old(*this);
  ++idx_;
  check_bounds();
  return old;
}

template <typename T>
typename rarray<T>::ptr &rarray<T>::ptr::operator--()
{
  --idx_;
  check_bounds();
  return *this;
}
template <typename T>
typename rarray<T>::ptr rarray<T>::ptr::operator--(int)
{
  ptr old(*this);
  --idx_;
  check_bounds();
  return old;
}

template <typename T>
typename rarray<T>::ptr &rarray<T>::ptr::operator+=(int x)
{
  idx_ += x;
  check_bounds();
  return *this;
}
template <typename T>
typename rarray<T>::ptr &rarray<T>::ptr::operator-=(int x)
{
  idx_ -= x;
  return *this;
}

template <typename T>
void rarray<T>::ptr::check_bounds()
{
  while (idx_ >= rb_.size_)
    rb_.resize(2 * rb_.size_);
}

template <typename T>
rarray<T>::ptr::ptr(rarray<T> &rb, size_t idx) : rb_(rb), idx_(idx)
{
  check_bounds();
}

template <typename T>
rarray<T>::ptr::ptr(const rarray<T>::ptr &p) : rb_(p.rb_), idx_(p.idx_) {}

template <typename T>
rarray<T>::rarray(size_t size_) : size_(size_)
{
  data_ = (T *)kmalloc(sizeof(T) * size_, NULL);
}

template <typename T>
rarray<T>::~rarray()
{
  kfree(data_);
}

template <typename T>
typename rarray<T>::ptr rarray<T>::get_ptr(size_t i)
{
  return ptr(*this, i);
}

template <typename T>
void rarray<T>::resize(size_t new_size)
{
  data_ = (T *)krealloc(data_, new_size);
  size_ = new_size;
}

template class rarray<char>;
