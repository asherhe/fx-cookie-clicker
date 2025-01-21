#ifndef COOKIE_LIST_H
#define COOKIE_LIST_H

#include <stddef.h>

/*
 * list class
 * similar to the STL list except with less features
 */

template <typename T>
class list
{
public:
  /* create a new empty list */
  list();

  /* create a new list using the values from another list */
  list(const list &other);

  /* copy the values from another list to this list */
  list &operator=(const list &other);

  /* deallocate all the memory used by this list */
  ~list();

private:
  struct ListNode
  {
    T val;
    ListNode *prev, *next;
    ListNode();
    ListNode(int v);
  } sentinel_;

  size_t size_;

public:
  /* iterator to allow us to traverse through the list */
  class iterator
  {
    friend class list;

  public:
    inline T &operator*() const { return node_->val; }
    iterator &operator++();
    iterator &operator--();
    bool operator==(const iterator &it);
    bool operator!=(const iterator &it);

  private:
    ListNode *node_;

    /* create a new list iterator with a given node */
    iterator(ListNode *node);
  };

public:
  /* iterator to first element of list */
  inline iterator begin() { return iterator(sentinel_.next); }
  /* iterator to end of list (note: not last element!) */
  inline iterator end() { return iterator(&sentinel_); }

  /* number of elements in this list */
  inline size_t size() const { return size_; }
  /* is the list empty? */
  inline bool empty() const { return size_ == 0; }

  /* first element in this list */
  inline T front() const { return sentinel_.next->val; }
  /* last element in this list */
  inline T back() const { return sentinel_.prev->val; }

  /* get the i-th element in this list */
  T at(size_t i);

  /* clear the contents of this list */
  void clear();

  /* insert a value `v` before `pos`. return a new iterator pointing to `v` */
  iterator insert(const iterator &pos, T v);

  /* erase the element `it` points to. return the iterator after the removed element */
  iterator erase(const iterator &it);

  /* add a value to the front of the list */
  void push_front(T v);
  /* add a value to the back of the list */
  void push_back(T v);
  /* remove the first element of the list */
  void pop_front();
  /* remove the last element of the list */
  void pop_back();
};

#endif // COOKIE_LIST_H