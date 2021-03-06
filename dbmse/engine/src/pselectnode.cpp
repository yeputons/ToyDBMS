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

PSelectNode::PSelectNode(LAbstractNode* p): PGetNextNode() {
  this->table = ((LSelectNode*)p)->GetBaseTable();
  this->prototype = p;
  stats_.rewound--;
  Rewind();
}

void PSelectNode::Rewind() {
  stats_.rewound++;
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
  stats_.output_blocks++;
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
      else if (prototype->fieldTypes[i] == VT_STRING)
        h = Value(word);
      else
        assert(false);
      tmp.push_back(h);
      i++;
    }
    if (p->predicate->check(tmp)) {
      stats_.output_rows++;
      result.push_back(tmp);
    }
  }
  if (!result.empty()) {
    stats_.non_empty_output_blocks++;
  }
  return result;
}

void PSelectNode::Print(int indent, bool print_stats) {
  for (int i = 0; i < indent; i++) {
    std::cout << " ";
  }
  LSelectNode* p = (LSelectNode*)prototype;
  std::cout << "SCAN " << table.relpath << " with predicate " << *p->predicate << "\n";
  if (print_stats) {
    for (int i = 0; i < indent; i++) std::cout << " ";
    std::cout << stats() << std::endl;
  }
  if (left != nullptr) left->Print(indent + 2, print_stats);
  if (right != nullptr) right->Print(indent + 2, print_stats);
}

