#ifndef LIST_H
#define LIST_H

#include <vector>
using namespace std;

template <class Node>
class List {
 public:
  void Append(Node* node) {
    if (tail_)
      tail_->after_ = node;
    else
      head_ = node;
    tail_ = node;
    node->after_ = nullptr;
    ++size_;
  }

  void Append(List* list) {
    if (tail_)
      tail_->after_ = list->head_;
    else
      head_ = list->head_;
    tail_ = list->tail_;
    size_ += list->size_;

    list->Clear();
  }

  vector<Node*> ToVector() {
    vector<Node*> nodes;
    for (auto* node : *this) nodes.push_back(node);
    Clear();
    return nodes;
  }

  int empty() const { return size_ == 0; }
  int size() const { return size_; }

  class Iterator {
   public:
    Iterator(Node* node) : node_(node), after_(node ? node->after_ : nullptr) {}
    void operator++() {
      node_ = after_;
      after_ = (node_ ? node_->after_ : nullptr);
    }
    bool operator!=(const Iterator& it) const { return node_ != it.node_; }
    Node* operator*() const { return node_; }

   private:
    Node* node_;
    Node* after_;
  };

  const Iterator begin() const { return Iterator(head_); }
  const Iterator end() const { return Iterator(nullptr); }

 private:
  void Clear() {
    head_ = tail_ = nullptr;
    size_ = 0;
  }

  Node* head_ = nullptr;
  Node* tail_ = nullptr;
  int size_ = 0;
};

#endif
