# libSAOCPPYield

This is my design pattern for implementing Python's `yield` statement in C/C++ (without the new C++2X `co_yield` stuff)

My original idea was to use (what I called) a `co_stack`; as we return through the normal stack, push frames onto the co_stack, which can be restored once we wish to yield the next item. This could be limited by having a fixed size co_stack, and is actually unnecessary.

Instead we store the locals of frames that we return through when yielding on the heap, which end up forming a linked list (yes I am aware of cache lines...). I invoke the help of `std::unique_ptr`, though all of this code could really be done in plain C. When restoring a frame we use `goto` to resume execution where we left off; we store a key which I call `on_restore_key` along with the local variables for this.

A typical invocation of an iterator of this style is

```cpp
Tree<int> t;
TreeIterator<int> i(&t);
int *yielded_int;
while (!(i.next(&yielded_int) < 0)) {
  std::cout << *yielded_int << std::endl;
}
```

`TreeIterator::next` returns `0` if an item was yielded, else `-1`.

## Building

I use [Meson](https://mesonbuild.com/) to build, with a top level Makefile for convenience.

One can run `make o3 && ./build/o3/test/main` to see my depth-first binary tree iterator in action
