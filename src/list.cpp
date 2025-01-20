#include "list.h"
#include <stddef.h>
// standard c++ memory allocation doesn't work, we have to use gint's
#include <gint/kmalloc.h>

template <typename T>
list<T>::list() : list_size(0)
{
  sentinel.prev = sentinel.next = &sentinel;
}

template <typename T>
list<T>::list(const list &other)
{
  sentinel.prev = sentinel.next = &sentinel;
  *this = other;
}

template <typename T>
list<T> &list<T>::operator=(const list &other)
{
  clear();
  list_size = other.list_size;
  if (list_size)
  {
    ListNode *curr = &sentinel, *copy = other.sentinel.next;
    while (copy != &other.sentinel)
    {
      ListNode *new_node = (ListNode *)kmalloc(sizeof(ListNode), NULL);
      new_node->val = copy->val;
      new_node->prev = curr;
      new_node->next = curr->next;
      curr->next = curr->next->prev = new_node;
      curr = new_node;
      copy = copy->next;
    }
  }
  return *this;
}

template <typename T>
list<T>::~list()
{
  clear();
}

template <typename T>
list<T>::ListNode::ListNode() {}

template <typename T>
list<T>::ListNode::ListNode(int v) : val(v) {}

template <typename T>
int &list<T>::iterator::operator*() { return node->val; }

template <typename T>
typename list<T>::iterator &list<T>::iterator::operator++()
{
  node = node->next;
  return *this;
}

template <typename T>
typename list<T>::iterator &list<T>::iterator::operator--()
{
  node = node->prev;
  return *this;
}

template <typename T>
bool list<T>::iterator::operator==(const iterator &it) { return node == it.node; }
template <typename T>
bool list<T>::iterator::operator!=(const iterator &it) { return node != it.node; }

template <typename T>
list<T>::iterator::iterator(ListNode *node) : node(node) {}

template <typename T>
typename list<T>::iterator list<T>::begin() { return iterator(sentinel.next); }
template <typename T>
typename list<T>::iterator list<T>::end() { return iterator(&sentinel); }

template <typename T>
size_t list<T>::size() { return list_size; }

template <typename T>
bool list<T>::empty() { return list_size == 0; }

template <typename T>
int list<T>::front() { return sentinel.next->val; }
template <typename T>
int list<T>::back() { return sentinel.prev->val; }

template <typename T>
int list<T>::at(int i)
{
  iterator it = begin();
  for (; i > 0; --i, ++it)
    ;
  return *it;
}

template <typename T>
void list<T>::clear()
{
  list_size = 0;
  ListNode *curr = sentinel.next, *next;
  for (; curr != &sentinel; curr = next)
  {
    next = curr;
    kfree(curr);
  }
}

template <typename T>
typename list<T>::iterator list<T>::insert(const iterator &pos, int v)
{
  ++list_size;

  ListNode *new_node = (ListNode *)kmalloc(sizeof(ListNode), NULL), *ins_pos = pos.node;
  new_node->val = v;
  new_node->prev = ins_pos->prev;
  new_node->next = ins_pos;
  ins_pos->prev = ins_pos->prev->next = new_node;
  return iterator(new_node);
}

template <typename T>
typename list<T>::iterator list<T>::erase(iterator it)
{
  --list_size;

  ListNode *node = it.node;
  node->prev->next = node->next;
  node->next->prev = node->prev;

  iterator ret_val(node->next);
  kfree(node);
  return ret_val;
}

template <typename T>
void list<T>::push_front(int v) { insert(begin(), v); }
template <typename T>
void list<T>::push_back(int v) { insert(end(), v); }
template <typename T>
void list<T>::pop_front() { erase(begin()); }
template <typename T>
void list<T>::pop_back() { erase(iterator(sentinel.prev)); }

template class list<int>;
