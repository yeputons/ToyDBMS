#include<cstddef>
#include<algorithm>
#include<utility>
#include<iostream>

#include "puniquenode.h"

PUniqueNode::PUniqueNode(std::unique_ptr<PGetNextNode> child_, LAbstractNode* p)
  : PGetNextNode(std::move(child_), nullptr, p) {
  Rewind();
}

void PUniqueNode::Rewind() {
  PGetNextNode* l = (PGetNextNode*)left.get();
  l->Rewind();
  past.clear();
}

std::vector<std::vector<Value>> PUniqueNode::GetNext() {
  PGetNextNode* l = (PGetNextNode*)left.get();
  std::vector<std::vector<Value>> data;
  std::vector<std::vector<Value>> lres = l->GetNext();
  while (!lres.empty()) {
    for (int i = 0; i < lres.size(); i++) {
      bool skip = false;
      for (int j = 0; j < past.size(); j++) {
        if (past[j] == lres[i]) { skip = true; break; }
      }
      if (!skip) {
        data.push_back(lres[i]);
        past.push_back(lres[i]);
      }
    }
    if (data.size() > BLOCK_SIZE) {
      break;
    } else {
      lres = l->GetNext();
    }
  }
  return data;
}

void PUniqueNode::Print(int indent) {
  for (int i = 0; i < indent; i++) {
    std::cout << " ";
  }
  std::cout << "UNIQUE:" << std::endl;
  left->Print(indent + 2);
}
