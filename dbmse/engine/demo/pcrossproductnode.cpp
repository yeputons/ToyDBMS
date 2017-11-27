#include<cstddef>
#include<algorithm>
#include<utility>
#include<iostream>

#include "pcrossproductnode.h"

PCrossProductNode::PCrossProductNode(std::unique_ptr<PGetNextNode> left_, std::unique_ptr<PGetNextNode> right_,
                     LAbstractNode* p): PGetNextNode(std::move(left_), std::move(right_), p) {
  Initialize();
}

std::vector<std::vector<Value>> PCrossProductNode::GetNext() {
  auto result = std::move(data);
  data.clear();
  return result;
}

void PCrossProductNode::Initialize() {
  PGetNextNode* l = (PGetNextNode*)left.get();
  PGetNextNode* r = (PGetNextNode*)right.get();
  std::vector<std::vector<Value>> lres = l->GetNext();
  std::vector<std::vector<Value>> rres = r->GetNext();
  for (const auto &lrow : lres)
    for (const auto &rrow : rres)  {
      std::vector<Value> result = lrow;
      result.insert(result.end(), rrow.begin(), rrow.end());
      data.push_back(result);
    }
}

void PCrossProductNode::Print(int indent) {
  for (int i = 0; i < indent; i++) {
    std::cout << " ";
  }
  std::cout << "CROSS-PRODUCT: " << std::endl;
  left->Print(indent + 2);
  right->Print(indent + 2);
}
