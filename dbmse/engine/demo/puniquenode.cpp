#include<cstddef>
#include<algorithm>
#include<utility>
#include<iostream>

#include "puniquenode.h"

PUniqueNode::PUniqueNode(std::unique_ptr<PGetNextNode> child_, LAbstractNode* p)
  : PGetNextNode(std::move(child_), nullptr, p) {
  Initialize();
}

std::vector<std::vector<Value>> PUniqueNode::GetNext() {
  return data;
}

void PUniqueNode::Initialize() {
  PGetNextNode* l = (PGetNextNode*)left.get();
  std::vector<std::vector<Value>> lres = l->GetNext();
  for (int i = 0; i < lres.size(); i++) {
    bool skip = false;
    for (int j = 0; j < i; j++) {
      if (lres[i] == lres[j]) { skip = true; break; }
    }
    if (!skip) {
      data.push_back(lres[i]);
    }
  }
}

void PUniqueNode::Print(int indent) {
  for (int i = 0; i < indent; i++) {
    std::cout << " ";
  }
  std::cout << "UNIQUE:" << std::endl;
  left->Print(indent + 2);
}
