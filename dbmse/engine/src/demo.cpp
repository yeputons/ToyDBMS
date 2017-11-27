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

#include <stdio.h>
#include <typeinfo>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <tuple>
#include "interface.h"
#include "basics.h"
#include "pselectnode.h"
#include "pcrossproductnode.h"
#include "pnestedloopjoinnode.h"
#include "pprojectnode.h"
#include "puniquenode.h"

std::unique_ptr<PGetNextNode> QueryFactory(LAbstractNode* node) {
  if (dynamic_cast<LSelectNode*>(node) != nullptr) {
    LSelectNode* tmp = (LSelectNode*)node;
    std::vector<Predicate> p;
    return std::unique_ptr<PSelectNode>(new PSelectNode(tmp, p));
  } else if (dynamic_cast<LJoinNode*>(node) != nullptr) {
    return std::unique_ptr<PNestedLoopJoinNode>(new PNestedLoopJoinNode(
    	QueryFactory(node->GetLeft()),
    	QueryFactory(node->GetRight()),
    	node));
  } else if (dynamic_cast<LCrossProductNode*>(node) != nullptr) {
    return std::unique_ptr<PCrossProductNode>(new PCrossProductNode(
    	QueryFactory(node->GetLeft()),
    	QueryFactory(node->GetRight()),
    	node));
  } else if (dynamic_cast<LProjectNode*>(node) != nullptr) {
    return std::unique_ptr<PProjectNode>(new PProjectNode(
        QueryFactory(node->GetLeft()),
        node));
  } else if (dynamic_cast<LUniqueNode*>(node) != nullptr) {
    return std::unique_ptr<PUniqueNode>(new PUniqueNode(
        QueryFactory(node->GetLeft()),
        node));
  } else
    return nullptr;
}

void ExecuteQuery(PGetNextNode* query) {
  std::vector<std::vector<Value>> res = query->GetNext();
  while (!res.empty()) {
    for (const auto &vals : res) {
      for (int i = 0; i < query->GetAttrNum(); i++) {
        if (vals[i].vtype == VT_INT)
          std::cout << vals[i].vint << " ";
        else if (vals[i].vtype == VT_STRING)
          std::cout << vals[i].vstr << " ";
      }
      printf("\n");
    }
    res = query->GetNext();
  }
}

int main() {
  {
    std::cout << "Starting demo" << std::endl;
    std::cout << "Query1: plain select" << std::endl;
    BaseTable bt1 = BaseTable("table1");
    std::cout << bt1;
    std::unique_ptr<LAbstractNode> n1(new LSelectNode(bt1, {}));
    std::unique_ptr<PGetNextNode> q1 = QueryFactory(n1.get());
    q1->Print(0);
    ExecuteQuery(q1.get());
  }

  {
    std::cout << std::endl << "Query2: simple equi-join" << std::endl;
    BaseTable bt1 = BaseTable("table1");
    BaseTable bt2 = BaseTable("table2");
    std::cout << bt1;
    std::cout << bt2;
    std::unique_ptr<LAbstractNode> n1(new LSelectNode(bt1, {}));
    std::unique_ptr<LAbstractNode> n2(new LSelectNode(bt2, {}));
    std::unique_ptr<LJoinNode> n3(new LJoinNode(std::move(n1), std::move(n2), "table1.id", "table2.id2", 666));
    std::unique_ptr<PGetNextNode> q1 = QueryFactory(n3.get());
    q1->Print(0);
    ExecuteQuery(q1.get());
  }

  {
    std::cout << std::endl << "Query3: simple cross-product" << std::endl;
    BaseTable bt1 = BaseTable("table1");
    BaseTable bt2 = BaseTable("table2");
    std::cout << bt1;
    std::cout << bt2;
    std::unique_ptr<LAbstractNode> n1(new LSelectNode(bt1, {}));
    std::unique_ptr<LAbstractNode> n2(new LSelectNode(bt2, {}));
    std::unique_ptr<LCrossProductNode> n3(new LCrossProductNode(std::move(n1), std::move(n2)));
    std::unique_ptr<PGetNextNode> q1 = QueryFactory(n3.get());
    q1->Print(0);
    ExecuteQuery(q1.get());
  }

  {
    std::cout << std::endl << "Query4: simple project of equi-join with predicate and unique" << std::endl;
    BaseTable bt1 = BaseTable("table1");
    BaseTable bt2 = BaseTable("table2");
    std::cout << bt1;
    std::cout << bt2;
    std::unique_ptr<LAbstractNode> n1(new LSelectNode(bt1, {Predicate(PT_GREATERTHAN, VT_INT, 2, 30, "")}));
    std::unique_ptr<LAbstractNode> n2(new LSelectNode(bt2, {}));
    std::unique_ptr<LJoinNode> n3(new LJoinNode(std::move(n1), std::move(n2), "table1.id", "table2.id2", 666));
    std::unique_ptr<LProjectNode> n4(new LProjectNode(std::move(n3), {"table2.type2", "table1.description"}));
    std::unique_ptr<LUniqueNode> n5(new LUniqueNode(std::move(n4)));
    std::unique_ptr<PGetNextNode> q1 = QueryFactory(n5.get());
    q1->Print(0);
    ExecuteQuery(q1.get());
  }
}
