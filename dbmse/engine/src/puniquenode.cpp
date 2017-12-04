#include<cstddef>
#include<algorithm>
#include<utility>
#include<iostream>

#include "puniquenode.h"

PUniqueNode::PUniqueNode(std::unique_ptr<PGetNextNode> child_, LAbstractNode* p)
  : PGetNextNode(std::move(child_), nullptr, p) {
  stats_.rewound--;
  Rewind();
}

void PUniqueNode::Rewind() {
  stats_.rewound++;
  PGetNextNode* l = (PGetNextNode*)left.get();
  l->Rewind();
  lres = l->GetNext();
  li = 0;
  past.clear();
}

std::vector<std::vector<Value>> PUniqueNode::GetNext() {
  stats_.output_blocks++;
  PGetNextNode* l = (PGetNextNode*)left.get();
  std::vector<std::vector<Value>> data;
  while (data.size() < BLOCK_SIZE && !lres.empty()) {
    if (li >= lres.size()) {
      lres = l->GetNext();
      li = 0;
      continue;
    }
    bool skip = false;
    for (int j = 0; j < past.size(); j++) {
      if (past[j] == lres[li]) { skip = true; break; }
    }
    if (!skip) {
      stats_.output_rows++;
      data.push_back(lres[li]);
      past.push_back(lres[li]);
    }
    li++;
  }
  if (!data.empty()) {
    stats_.non_empty_output_blocks++;
  }
  return data;
}

void PUniqueNode::Print(int indent, bool print_stats) {
  for (int i = 0; i < indent; i++) {
    std::cout << " ";
  }
  std::cout << "UNIQUE:" << std::endl;
  if (print_stats) {
    for (int i = 0; i < indent; i++) std::cout << " ";
    std::cout << stats() << std::endl;
  }
  left->Print(indent + 2, print_stats);
}
