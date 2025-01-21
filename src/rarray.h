#ifndef COOKIE_RARRAY_H
#define COOKIE_RARRAY_H

#include <stddef.h>

/*
 * resizing array
 * an array structure that can be resized if we need more space
 * mostly used for managing buffers of unknown length
 */
template <typename T>
class rarray
{
private:
  T *data_;
  size_t size_;

public:
  /* pointer class that represents an index in the resizing bytearray */
  class ptr
  {
  public:
    ptr(const ptr &p);
    ptr &operator=(const ptr &p);

    /* get the pointer that this ptr wraps */
    inline operator T *() const { return rb_.data_ + idx_; }

    /* get the current index of this pointer */
    inline size_t index() const { return idx_; }

    /* the value this ptr is pointing to */
    inline T &operator*() const { return rb_.data_[idx_]; }

    /*
     * difference between this and another pointer
     * NOTE: no error handling for if we have ptrs from different rarrays!
     */
    inline size_t operator-(const ptr &p) const { return idx_ - p.idx_; }

    inline ptr operator+(int x) const { return ptr(rb_, idx_ + x); }
    inline ptr operator-(int x) const { return ptr(rb_, idx_ - x); }
    ptr &operator++();
    ptr operator++(int);
    ptr &operator--();
    ptr operator--(int);
    ptr &operator+=(int x);
    ptr &operator-=(int x);

  private:
    /* resizing array that this pointer belongs to */
    rarray &rb_;
    /* array index this ptr points to in rb_.data_ */
    size_t idx_;

    friend rarray;
    ptr(rarray &rb, size_t idx);

    /* check if this ptr is within bounds and resize() the bytearray if necessary */
    void check_bounds();
  };

  /* create a new resizing array of size `initial_size` */
  rarray(size_t size);
  /* destroy this resizing array and free up used memory */
  ~rarray();

  inline T &operator*() const { return data_[0]; }
  inline T &operator[](size_t i) const { return data_[i]; }
  /* make a new pointer at a given index */
  ptr get_ptr(size_t i);

  /* get rarray data */
  inline T *data() const { return data_; }
  /* get rarray size */
  inline size_t size() const { return size_; }

  /* change the size of this array */
  void resize(size_t new_size);
};

#endif // COOKIE_RARRAY_H