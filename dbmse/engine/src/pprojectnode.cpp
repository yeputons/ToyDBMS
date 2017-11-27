#include<cstddef>
#include<algorithm>
#include<utility>
#include<iostream>

#include "pprojectnode.h"

PProjectNode::PProjectNode(std::unique_ptr<PGetNextNode> child_, LAbstractNode* p)
  : PGetNextNode(std::move(child_), nullptr, p) {
  LProjectNode* pn = (LProjectNode*)p;

  const auto& childFieldNames = p->GetLeft()->fieldNames;
  for (const auto &field : pn->offsets) {
    for (int i = 0; i < childFieldNames.size(); i++) {
      const auto& curNames = childFieldNames[i];
      if (std::find(curNames.begin(), curNames.end(), field) != curNames.end()) {
        indices.push_back(i);
      }
    }
  }
}


void PProjectNode::Rewind() {
  PGetNextNode* l = (PGetNextNode*)left.get();
  l->Rewind();
  stats_.rewound++;
}

std::vector<std::vector<Value>> PProjectNode::GetNext() {
  stats_.output_blocks++;
  PGetNextNode* l = (PGetNextNode*)left.get();
  std::vector<std::vector<Value>> data;
  std::vector<std::vector<Value>> lres = l->GetNext();
  while (!lres.empty()) {
    for (const auto &lrow : lres) {
      std::vector<Value> result;
      for (int idx : indices)
        result.push_back(lrow[idx]);
      data.push_back(result);
      stats_.output_rows++;
    }
    if (!data.empty()) {
      break;
    } else {
      lres = l->GetNext();
    }
  }
  if (!data.empty()) {
    stats_.non_empty_output_blocks++;
  }
  return data;
}

void PProjectNode::Print(int indent, bool print_stats) {
  for (int i = 0; i < indent; i++) {
    std::cout << " ";
  }
  std::cout << "PROJECT:";
  for (int idx : indices)
    std::cout << " " << idx << "(" << prototype->GetLeft()->fieldNames[idx][0] << ")";
  std::cout << std::endl;
  if (print_stats) {
    for (int i = 0; i < indent; i++) std::cout << " ";
    std::cout << stats() << std::endl;
  }
  left->Print(indent + 2, print_stats);
}
