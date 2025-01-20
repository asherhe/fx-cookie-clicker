#ifndef COOKIE_LIST_H
#define COOKIE_LIST_H

#include <stddef.h>

/*
 * list class
 * similar to the STL list except with less features
 * apparently that doesn't work here either
 *
 * templates also seem to really mess with malloc() so this list is restricted to the int type
 */

class list_int
{
public:
  /* create a new empty list */
  list_int();

  /* create a new list using the values from another list */
  list_int(const list_int &other);

  /* copy the values from another list to this list */
  list_int &operator=(const list_int &other);

  /* deallocate all the memory used by this list */
  ~list_int();

private:
  struct ListNode
  {
    int val;
    ListNode *prev, *next;
    ListNode();
    ListNode(int v);
  } sentinel;

  size_t list_size;

public:
  class iterator
  {
    friend class list_int;

  public:
    int &operator*();
    iterator &operator++();
    iterator &operator--();
    bool operator==(const iterator &it);
    bool operator!=(const iterator &it);

  private:
    ListNode *node;

    /* create a new list iterator with a given node */
    iterator(ListNode *node);
  };

public:
  /* iterator to first element of list */
  iterator begin();
  /* iterator to end of list (note: not last element!) */
  iterator end();

  /* number of elements in this list */
  size_t size();
  /* is the list empty? */
  bool empty();

  /* first element in this list */
  int front();
  /* last element in this list */
  int back();
  /* get the i-th element in this list */
  int at(int i);

  /* clear the contents of this list */
  void clear();

  /* insert a value `v` before `pos`. return a new iterator pointing to `v` */
  iterator insert(const iterator &pos, int v);

  /* erase the element `it` points to. return the iterator after the removed element */
  iterator erase(iterator it);

  /* add a value to the front of the list */
  void push_front(int v);
  /* add a value to the back of the list */
  void push_back(int v);
  /* remove the first element of the list */
  void pop_front();
  /* remove the last element of the list */
  void pop_back();
};

#endif // COOKIE_LIST_H