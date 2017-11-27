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

#include <iostream>
#include <fstream>
#include <string>

#include <string.h>
#include <vector>
#include <sstream>

#include "pselectnode.h"

PSelectNode::PSelectNode() {}

PSelectNode::PSelectNode(LAbstractNode* p, std::vector<Predicate> predicate): PGetNextNode() {
  this->table = ((LSelectNode*)p)->GetBaseTable();
  this->predicate = predicate;
  this->prototype = p;
  Rewind();
}

void PSelectNode::Rewind() {
  f.close();
  f.open(table.relpath);
  if (f.is_open()) {
    std::string line;
    // skipping first 4 lines
    getline(f, line);
    getline(f, line);
    getline(f, line);
    getline(f, line);
  } else {
    std::cout << "Unable to open file";
  }
}

std::vector<std::vector<Value>> PSelectNode::GetNext() {
  std::string line;
  LSelectNode* p = (LSelectNode*)prototype;
  std::vector<std::vector<Value>> result;

  while (result.size() < BLOCK_SIZE && getline(f, line)) {
    std::vector<Value> tmp;
    std::istringstream iss(line, std::istringstream::in);
    int i = 0;
    std::string word;
    while (iss >> word) {
      // Yeah, no predicates :) -- Homework
      Value h;
      if (prototype->fieldTypes[i] == VT_INT)
        h = Value(std::stoi(word));
      else
        h = Value(word);
      tmp.push_back(h);
      i++;
    }
    p->ResetIterator();
    bool to_select = true;
    for (;;) {
      int stop;
      Predicate pred;
      std::tie(stop, pred) = p->GetNextPredicate();
      if (stop) break;
      to_select &= pred.check(tmp[pred.attribute]);
    }
    if (to_select) {
      result.push_back(tmp);
    }
  }
  return result;
}

void PSelectNode::Print(int indent) {
  for (int i = 0; i < indent; i++) {
    std::cout << " ";
  }
  std::cout << "SCAN " << table.relpath << " with predicate ";
  if (predicate.size() != 0)
    std::cout << predicate[0];
  else
    std::cout << "NULL" << std::endl;
  if (left != nullptr) left->Print(indent + 2);
  if (right != nullptr) right->Print(indent + 2);
}

