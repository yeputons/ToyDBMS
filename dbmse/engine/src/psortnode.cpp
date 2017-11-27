#include<cstddef>
#include<algorithm>
#include<utility>
#include<iostream>

#include "psortnode.h"

PSortNode::PSortNode(std::unique_ptr<PGetNextNode> child_, LAbstractNode* p)
  : PGetNextNode(std::move(child_), nullptr, p) {
}

void PSortNode::Rewind() {
  stats_.rewound++;
  PGetNextNode* l = (PGetNextNode*)left.get();
  l->Rewind();
  data.clear();
}

std::vector<std::vector<Value>> PSortNode::GetNext() {
  stats_.output_blocks++;
  LSortNode *p = (LSortNode*)prototype;
  PGetNextNode* l = (PGetNextNode*)left.get();
  std::vector<std::vector<Value>> lres = l->GetNext();
  if (!lres.empty()) {
    while (!lres.empty()) {
      data.insert(data.end(), lres.begin(), lres.end());
      lres = l->GetNext();
    }
    sort(data.begin(), data.end(), [p](const std::vector<Value> &a, const std::vector<Value> &b) {
      return a[p->offset] < b[p->offset];
    });
  }
  auto mid = std::min(data.end(), data.begin() + BLOCK_SIZE);
  std::vector<std::vector<Value>> slice(data.begin(), mid);
  data.erase(data.begin(), mid);
  stats_.output_rows += slice.size();
  if (!slice.empty()) {
    stats_.non_empty_output_blocks++;
  }
  return slice;
}

void PSortNode::Print(int indent, bool print_stats) {
  for (int i = 0; i < indent; i++) {
    std::cout << " ";
  }
  LSortNode *p = (LSortNode*)prototype;
  std::cout << "SORT:" << p->offset << " (" << p->GetLeft()->fieldNames[p->offset][0] << ")" << std::endl;
  if (print_stats) {
    for (int i = 0; i < indent; i++) std::cout << " ";
    std::cout << stats() << std::endl;
  }
  left->Print(indent + 2, print_stats);
}
