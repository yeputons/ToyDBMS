#include<cstddef>
#include<algorithm>
#include<utility>
#include<iostream>

#include "pcrossproductnode.h"

PCrossProductNode::PCrossProductNode(std::unique_ptr<PGetNextNode> left_, std::unique_ptr<PGetNextNode> right_,
                     LAbstractNode* p): PGetNextNode(std::move(left_), std::move(right_), p) {
  Rewind();
}

void PCrossProductNode::Rewind() {
  PGetNextNode* l = (PGetNextNode*)left.get();
  PGetNextNode* r = (PGetNextNode*)right.get();
  l->Rewind();
  r->Rewind();
  lres = l->GetNext();
  rres = r->GetNext();
  lptr = 0;
}

std::vector<std::vector<Value>> PCrossProductNode::GetNext() {
  PGetNextNode* l = (PGetNextNode*)left.get();
  PGetNextNode* r = (PGetNextNode*)right.get();
  std::vector<std::vector<Value>> data;
  if (lres.empty()) {
    return data;
  }
  while (data.empty()) {
    const auto &lrow = lres[lptr];
    for (const auto &rrow : rres) {
      std::vector<Value> result = lrow;
      result.insert(result.end(), rrow.begin(), rrow.end());
      data.push_back(result);
    }

    rres = r->GetNext();
    if (rres.empty()) {
      r->Rewind();
      rres = r->GetNext();
      lptr++;
      if (lptr == lres.size()) {
        lptr = 0;
        lres = l->GetNext();
        if (lres.empty()) {
          break;
        }
      }
    }
  }
  return data;
}

void PCrossProductNode::Print(int indent) {
  for (int i = 0; i < indent; i++) {
    std::cout << " ";
  }
  std::cout << "CROSS-PRODUCT: " << std::endl;
  left->Print(indent + 2);
  right->Print(indent + 2);
}
