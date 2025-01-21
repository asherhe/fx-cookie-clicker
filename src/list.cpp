#include "list.h"
#include <stddef.h>
// standard c++ memory allocation doesn't work, we have to use gint's
#include <gint/kmalloc.h>

template <typename T>
list<T>::list() : size_(0)
{
  sentinel_.prev = sentinel_.next = &sentinel_;
}

template <typename T>
list<T>::list(const list &other)
{
  sentinel_.prev = sentinel_.next = &sentinel_;
  *this = other;
}

template <typename T>
list<T> &list<T>::operator=(const list &other)
{
  clear();
  size_ = other.size_;
  if (size_)
  {
    ListNode *curr = &sentinel_, *copy = other.sentinel_.next;
    while (copy != &other.sentinel_)
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
typename list<T>::iterator &list<T>::iterator::operator++()
{
  node_ = node_->next;
  return *this;
}

template <typename T>
typename list<T>::iterator &list<T>::iterator::operator--()
{
  node_ = node_->prev;
  return *this;
}

template <typename T>
bool list<T>::iterator::operator==(const iterator &it) { return node_ == it.node_; }
template <typename T>
bool list<T>::iterator::operator!=(const iterator &it) { return node_ != it.node_; }

template <typename T>
list<T>::iterator::iterator(ListNode *node) : node_(node) {}

template <typename T>
T list<T>::at(size_t i)
{
  iterator it = begin();
  for (; i > 0; --i, ++it)
    ;
  return *it;
}

template <typename T>
void list<T>::clear()
{
  size_ = 0;
  ListNode *curr = sentinel_.next, *next;
  for (; curr != &sentinel_; curr = next)
  {
    next = curr;
    kfree(curr);
  }
}

template <typename T>
typename list<T>::iterator list<T>::insert(const iterator &pos, T v)
{
  ++size_;

  ListNode *new_node = (ListNode *)kmalloc(sizeof(ListNode), NULL), *ins_pos = pos.node_;
  new_node->val = v;
  new_node->prev = ins_pos->prev;
  new_node->next = ins_pos;
  ins_pos->prev = ins_pos->prev->next = new_node;
  return iterator(new_node);
}

template <typename T>
typename list<T>::iterator list<T>::erase(const iterator &it)
{
  --size_;

  ListNode *node = it.node_;
  node->prev->next = node->next;
  node->next->prev = node->prev;

  iterator ret_val(node->next);
  kfree(node);
  return ret_val;
}

template <typename T>
void list<T>::push_front(T v) { insert(begin(), v); }
template <typename T>
void list<T>::push_back(T v) { insert(end(), v); }
template <typename T>
void list<T>::pop_front() { erase(begin()); }
template <typename T>
void list<T>::pop_back() { erase(iterator(sentinel_.prev)); }

template class list<int>;
