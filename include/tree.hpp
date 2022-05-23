#ifndef LIBSAOCPPYIELD__TREE_HPP
#define LIBSAOCPPYIELD__TREE_HPP

#include <memory>
#include <vector>
#include <iostream>

namespace SAO::Tree {

enum TreeDirection {
  LEFT,
  RIGHT
};

template <typename T>
struct TreeNode;

template <typename T>
struct Tree;

enum TreeIteratorOnRestoreKey {
  self,
  left,
  right,
};

template <typename T>
struct TreeIteratorFrame;

template <typename T>
struct TreeIterator;

template <typename T>
struct TreeNode {
  std::unique_ptr<TreeNode<T>> left;
  std::unique_ptr<TreeNode<T>> right;
  std::unique_ptr<T> data;

  TreeNode(std::unique_ptr<T> data) :
    left(nullptr),
    right(nullptr),
    data(std::move(data)) {
  }

  TreeNode(
    std::unique_ptr<TreeNode<T>> left,
    std::unique_ptr<TreeNode<T>> right,
    std::unique_ptr<T> data
  ) :
    left(std::move(left)),
    right(std::move(right)),
    data(std::move(data)) {
  }

  /* Static methods for Tree to call indirectly on &this->contents
   * Good taste recursive methods
   */

  static int depth(TreeNode<T> *thiss) {
    if (!thiss) {
      return 0;
    }

    return 1 + std::max(
      depth(thiss->left.get()),
      depth(thiss->right.get())
    );
  }

  static void print(TreeNode<T> *thiss, const int prefix_length, std::ostream &os) {
    if (!thiss) {
      return;
    }

    print(thiss->right.get(), prefix_length + 1, os);

    for (int i = 0; i < prefix_length; i++) {
      os << "\t";
    }
    os << *thiss->data << std::endl;

    print(thiss->left.get(), prefix_length + 1, os);
  }

  static void add_entry(
    std::unique_ptr<TreeNode<T>> *p_this,
    std::unique_ptr<T> data,
    const std::vector<TreeDirection> &seq,
    const TreeDirection move_existing_to
  ) {
    for (const auto &c : seq) {
      switch (c) {
        case LEFT: {
          p_this = &(*p_this)->left;
          break;
        }
        case RIGHT: {
          p_this = &(*p_this)->right;
          break;
        }
      }
    }
    switch (move_existing_to) {
      case LEFT: {
        *p_this = std::make_unique<TreeNode<T>>(std::move(*p_this), nullptr, std::move(data));
        break;
      }
      case RIGHT: {
        *p_this = std::make_unique<TreeNode<T>>(nullptr, std::move(*p_this), std::move(data));
        break;
      }
    }
  }

  static void add_entry(
    std::unique_ptr<TreeNode<T>> *p_this,
    std::unique_ptr<T> data
  ) {
    if (!*p_this) {
      *p_this = std::make_unique<TreeNode<T>>(std::move(data));
    } else if (*data < *(*p_this)->data) {
      add_entry(&(*p_this)->left, std::move(data));
    } else {
      add_entry(&(*p_this)->right, std::move(data));
    }
  }

  static void invert(TreeNode<T> *thiss) {
    if (!thiss) {
      return;
    }

    thiss->left.swap(thiss->right);

    invert(thiss->left.get());
    invert(thiss->right.get());
  }

};

template <typename T>
struct Tree {
  std::unique_ptr<TreeNode<T>> contents;

  Tree() : contents(nullptr) {}

  int depth() {
    return TreeNode<T>::depth(this->contents.get());
  }

  void print(std::ostream &os) {
    TreeNode<T>::print(this->contents.get(), 0, os);
  }

  void add_entry(std::unique_ptr<T> data, const std::vector<TreeDirection> &seq, TreeDirection move_existing_to) {
    TreeNode<T>::add_entry(&this->contents, std::move(data), seq, move_existing_to);
  }

  /* Useful when we know we're adding a tail node and we don't care about where the overwritten nullptr is moved to */
  void add_entry(std::unique_ptr<T> data, const std::vector<TreeDirection> &seq) {
    add_entry(std::move(data), seq, LEFT);
  }

  void add_entry(std::unique_ptr<T> data) {
    TreeNode<T>::add_entry(&this->contents, std::move(data));
  }

  void operator+=(std::unique_ptr<T> data) {
    add_entry(std::move(data));
  }

  void invert() {
    TreeNode<T>::invert(this->contents.get());
  }

};

template <typename T>
struct TreeIteratorFrame {
  TreeIteratorOnRestoreKey on_restore_key;
  TreeNode<T> *node_to_yield_from;
  TreeIterator<T> left_iterator;
  TreeIterator<T> right_iterator;

  static std::unique_ptr<TreeIteratorFrame<T>> yield(
    std::unique_ptr<TreeIteratorFrame<T>> frame_locals,
    T **p_yield_to
  ) {
    switch (frame_locals->on_restore_key) {
      case TreeIteratorOnRestoreKey::self: {
        goto start_self;
      }
      case TreeIteratorOnRestoreKey::left: {
        goto start_left;
      }
      case TreeIteratorOnRestoreKey::right: {
        goto start_right;
      }
      default: {
        /* never reached */
        return nullptr;
      }
    }

    start_self:
    if (!frame_locals->node_to_yield_from) {
      /* no node here */
      return nullptr;
    }

    /* yield this node */
    *p_yield_to = frame_locals->node_to_yield_from->data.get();

    /* next time yield from the left subtree */
    frame_locals->left_iterator = TreeIterator<T>(frame_locals->node_to_yield_from->left.get());
    frame_locals->on_restore_key = TreeIteratorOnRestoreKey::left;
    return frame_locals;

    start_left:
    while (!(frame_locals->left_iterator.next(p_yield_to) < 0)) {
      /* yield from left subtree */
      return frame_locals;
    }

    /* end of left subtree; now yield from right subtree */
    frame_locals->right_iterator = TreeIterator<T>(frame_locals->node_to_yield_from->right.get());
    frame_locals->on_restore_key = TreeIteratorOnRestoreKey::right;

    start_right:
    while (!(frame_locals->right_iterator.next(p_yield_to) < 0)) {
      /* yield from right subtree */
      return frame_locals;
    }

    /* end of right subtree; end of node */
    return nullptr;
  }
};

template <typename T>
struct TreeIterator {
  std::unique_ptr<TreeIteratorFrame<T>> stored_frame;

  TreeIterator() {}

  TreeIterator(TreeNode<T> *node) : stored_frame(
    std::unique_ptr<TreeIteratorFrame<T>>(new TreeIteratorFrame<T>{
      TreeIteratorOnRestoreKey::self,
      node,
      {},
      {}
      })
  ) {
  }

  /* construct an iterator that will iterate depth-first through tree */
  TreeIterator(Tree<T> *tree) : TreeIterator(tree->contents.get()) {}

  /* yield the next element of the iterator into p_yield_to */
  int next(T **p_yield_to) {
    stored_frame = TreeIteratorFrame<T>::yield(std::move(stored_frame), p_yield_to);
    if (!stored_frame) {
      return -1;
    } else {
      return 0;
    }
  }
};

}

#endif
