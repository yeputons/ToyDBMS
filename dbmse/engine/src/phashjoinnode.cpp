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

#include "phashjoinnode.h"

PHashJoinNode::PHashJoinNode(std::unique_ptr<PGetNextNode> left_, std::unique_ptr<PGetNextNode> right_,
                     LAbstractNode* p_): PGetNextNode(std::move(left_), std::move(right_), p_) {
  PGetNextNode* l = (PGetNextNode*)left.get();
  PGetNextNode* r = (PGetNextNode*)right.get();
  LAbstractNode* lp = l->prototype;
  LAbstractNode* rp = r->prototype;
  std::vector<std::vector<std::string>> ln = lp->fieldNames;
  std::vector<std::vector<std::string>> rn = rp->fieldNames;

  for (int i = 0; i < lp->fieldNames.size(); i++) {
    if (std::find(ln[i].begin(), ln[i].end(), ((LJoinNode*)prototype)->offset1) != ln[i].end()) {
      lpos = i;
      break;
    }
    if (std::find(ln[i].begin(), ln[i].end(), ((LJoinNode*)prototype)->offset2) != ln[i].end()) {
      lpos = i;
      break;
    }
  }

  for (int i = 0; i < rp->fieldNames.size(); i++) {
    if (std::find(rn[i].begin(), rn[i].end(), ((LJoinNode*)prototype)->offset1) != rn[i].end()) {
      rpos = i;
      break;
    }
    if (std::find(rn[i].begin(), rn[i].end(), ((LJoinNode*)prototype)->offset2) != rn[i].end()) {
      rpos = i;
      break;
    }
  }

  vt = lp->fieldTypes[lpos];

  Rewind();
}

void PHashJoinNode::Rewind() {
  PGetNextNode* l = (PGetNextNode*)left.get();
  PGetNextNode* r = (PGetNextNode*)right.get();
  l->Rewind();
  r->Rewind();
  rhs.clear();
}

std::vector<std::vector<Value>> PHashJoinNode::GetNext() {
  PGetNextNode* l = (PGetNextNode*)left.get();
  PGetNextNode* r = (PGetNextNode*)right.get();
  std::vector<std::vector<Value>> rres = r->GetNext();
  while (!rres.empty()) {
    for (const auto &rrow : rres) {
      rhs.emplace(rrow[rpos], rrow);
    }
    rres = r->GetNext();
  }

  std::vector<std::vector<Value>> result;
  while (result.size() < BLOCK_SIZE) {
    auto lres = l->GetNext();
    if (lres.empty()) {
      break;
    }
    for (const auto &lrow : lres) {
      auto it = rhs.equal_range(lrow[lpos]);
      for (auto x = it.first; x != it.second; x++) {
        const auto &rrow = x->second;
        assert(lrow[lpos] == rrow[rpos]);
        std::vector<Value> tmp;
        for (int k = 0; k < lrow.size(); k++)
          if (k != lpos)
            tmp.push_back(lrow[k]);
        for (int k = 0; k < rrow.size(); k++)
          if (k != rpos)
            tmp.push_back(rrow[k]);
        tmp.push_back(lrow[lpos]);
        result.push_back(tmp);
      }
    }
  }
  return result;
}

void PHashJoinNode::Print(int indent) {
  for (int i = 0; i < indent; i++) {
    std::cout << " ";
  }
  std::cout << "SS-JOIN: " << ((LJoinNode*)prototype)->offset1 << "=" << ((LJoinNode*)prototype)->offset2 << std::endl;
  left->Print(indent + 2);
  right->Print(indent + 2);
}
