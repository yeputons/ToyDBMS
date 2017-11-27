// Toy DBMS, v0.4
// George Chernishev (c) 2016-2017, chernishev<at>google mail
// A task framework for undergraduate students at Saint-Petersburg Academic University, DBMS development course
// More details regarding the course can be found here: www.math.spbu.ru/user/chernishev/
// CHANGELOG:
// 0.4: no chance for efficiency competition, so, this year I reoriented task towards ideas:
//      1) tried to remove C code, now we are using lots of std:: stuff
//      2) implemented support for multiple attributes in the DBMS
//      3) code clean-up and restructurization
// 0.3: added:
//      1) support for restricting physical join node size
//      2) support for deduplication node, LUniqueNode
//      3) print methods for Predicate and BaseTable
//      updated:
//      1) new format for data files: third line is the sort status now
//      2) added projection code
//      3) contract contains print methods for physical and logical nodes
// 0.2: first public release

#include<cstddef>
#include<algorithm>
#include<utility>

#include "punionnode.h"

PUnionNode::PUnionNode(std::unique_ptr<PGetNextNode> left_, std::unique_ptr<PGetNextNode> right_,
                     LAbstractNode* p_): PGetNextNode(std::move(left_), std::move(right_), p_) {
  Rewind();
}

void PUnionNode::Rewind() {
  PGetNextNode* l = (PGetNextNode*)left.get();
  PGetNextNode* r = (PGetNextNode*)right.get();
  l->Rewind();
  r->Rewind();
}

std::vector<std::vector<Value>> PUnionNode::GetNext() {
  PGetNextNode* l = (PGetNextNode*)left.get();
  PGetNextNode* r = (PGetNextNode*)right.get();
  std::vector<std::vector<Value>> data;
  while (data.size() < BLOCK_SIZE) {
    std::vector<std::vector<Value>> lres = l->GetNext();
    if (lres.empty()) {
      break;
    }
    data.insert(data.end(), lres.begin(), lres.end());
  }
  while (data.size() < BLOCK_SIZE) {
    std::vector<std::vector<Value>> rres = r->GetNext();
    if (rres.empty()) {
      break;
    }
    data.insert(data.end(), rres.begin(), rres.end());
  }
  return data;
}

void PUnionNode::Print(int indent) {
  for (int i = 0; i < indent; i++) {
    std::cout << " ";
  }
  std::cout << "UNION:" << std::endl;
  left->Print(indent + 2);
  right->Print(indent + 2);
}
