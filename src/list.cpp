#include "list.h"
// standard c++ memory allocation doesn't work, we have to use gint's
#include <gint/kmalloc.h>

list_int::list_int() : list_size(0)
{
  sentinel.prev = sentinel.next = &sentinel;
}

list_int::list_int(const list_int &other)
{
  sentinel.prev = sentinel.next = &sentinel;
  *this = other;
}

list_int &list_int::operator=(const list_int &other)
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

list_int::~list_int()
{
  clear();
}

list_int::ListNode::ListNode() {}
list_int::ListNode::ListNode(int v) : val(v) {}

int &list_int::iterator::operator*() { return node->val; }

list_int::iterator &list_int::iterator::operator++()
{
  node = node->next;
  return *this;
}

list_int::iterator &list_int::iterator::operator--()
{
  node = node->prev;
  return *this;
}

bool list_int::iterator::operator==(const iterator &it) { return node == it.node; }
bool list_int::iterator::operator!=(const iterator &it) { return node != it.node; }

list_int::iterator::iterator(ListNode *node) : node(node) {}

list_int::iterator list_int::begin() { return iterator(sentinel.next); }
list_int::iterator list_int::end() { return iterator(&sentinel); }

unsigned list_int::size() { return list_size; }
bool list_int::empty() { return list_size == 0; }

int list_int::front() { return sentinel.next->val; }
int list_int::back() { return sentinel.prev->val; }
int list_int::at(int i)
{
  iterator it = begin();
  for (; i > 0; --i, ++it)
    ;
  return *it;
}

void list_int::clear()
{
  list_size = 0;
  ListNode *curr = sentinel.next, *next;
  for (; curr != &sentinel; curr = next)
  {
    next = curr;
    kfree(curr);
  }
}

list_int::iterator list_int::insert(const iterator &pos, int v)
{
  ++list_size;

  ListNode *new_node = (ListNode *)kmalloc(sizeof(ListNode), NULL), *ins_pos = pos.node;
  new_node->val = v;
  new_node->prev = ins_pos->prev;
  new_node->next = ins_pos;
  ins_pos->prev = ins_pos->prev->next = new_node;
  return iterator(new_node);
}

list_int::iterator list_int::erase(iterator it)
{
  --list_size;

  ListNode *node = it.node;
  node->prev->next = node->next;
  node->next->prev = node->prev;

  iterator ret_val(node->next);
  kfree(node);
  return ret_val;
}

void list_int::push_front(int v) { insert(begin(), v); }
void list_int::push_back(int v) { insert(end(), v); }
void list_int::pop_front() { erase(begin()); }
void list_int::pop_back() { erase(iterator(sentinel.prev)); }
