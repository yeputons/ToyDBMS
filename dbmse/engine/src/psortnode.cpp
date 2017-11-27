#include<cstddef>
#include<algorithm>
#include<utility>
#include<iostream>

#include "psortnode.h"

PSortNode::PSortNode(std::unique_ptr<PGetNextNode> child_, LAbstractNode* p)
  : PGetNextNode(std::move(child_), nullptr, p) {
  Rewind();
}

void PSortNode::Rewind() {
  PGetNextNode* l = (PGetNextNode*)left.get();
  l->Rewind();
  data.clear();
}

std::vector<std::vector<Value>> PSortNode::GetNext() {
  PGetNextNode* l = (PGetNextNode*)left.get();
  std::vector<std::vector<Value>> lres = l->GetNext();
  if (!lres.empty()) {
    while (!lres.empty()) {
      data.insert(data.end(), lres.begin(), lres.end());
      lres = l->GetNext();
    }
    sort(data.begin(), data.end());
  }
  auto res = data;
  data.clear();
  return res;
  auto mid = std::min(data.end(), data.begin() + BLOCK_SIZE);
  std::vector<std::vector<Value>> slice(data.begin(), mid);
  data.erase(mid, data.end());
  return slice;
}

void PSortNode::Print(int indent) {
  for (int i = 0; i < indent; i++) {
    std::cout << " ";
  }
  std::cout << "SORT:" << std::endl;
  left->Print(indent + 2);
}
