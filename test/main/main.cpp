#include <iostream>

#include "tree.hpp"

using namespace SAO::Tree;

int main() {
  Tree<int> t;

  t.add_entry(std::make_unique<int>(3), std::vector<TreeDirection>({}));
  t.add_entry(std::make_unique<int>(4), std::vector<TreeDirection>({ LEFT }));
  t.add_entry(std::make_unique<int>(5), std::vector<TreeDirection>({ RIGHT }));
  t.add_entry(std::make_unique<int>(6), std::vector<TreeDirection>({ LEFT, LEFT }));
  t.add_entry(std::make_unique<int>(7), std::vector<TreeDirection>({ LEFT, RIGHT }));
  t.add_entry(std::make_unique<int>(8), std::vector<TreeDirection>({ RIGHT, LEFT }));
  t.add_entry(std::make_unique<int>(9), std::vector<TreeDirection>({ RIGHT, RIGHT }));
  t.add_entry(std::make_unique<int>(20), std::vector<TreeDirection>({ RIGHT, RIGHT, LEFT }));
  t.add_entry(std::make_unique<int>(21), std::vector<TreeDirection>({ RIGHT, RIGHT, RIGHT }));
  t.add_entry(std::make_unique<int>(90), std::vector<TreeDirection>({ RIGHT, RIGHT, RIGHT }));
  t.add_entry(std::make_unique<int>(91), std::vector<TreeDirection>({ RIGHT, RIGHT, RIGHT, RIGHT }));
  t.add_entry(std::make_unique<int>(92), std::vector<TreeDirection>({ RIGHT, RIGHT, RIGHT, RIGHT, LEFT }));
  t.add_entry(std::make_unique<int>(93), std::vector<TreeDirection>({ RIGHT, RIGHT, RIGHT, RIGHT, RIGHT }));

  t.print(std::cout);
  std::cout << std::endl;

  TreeIterator<int> i(&t);
  int *yielded_int;
  while (!(i.next(&yielded_int) < 0)) {
    std::cout << *yielded_int << std::endl;
  }

  return 0;
}
