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
#include "psortnode.h"
#include "psortedjoinnode.h"
#include "phashjoinnode.h"
#include "punionnode.h"

std::unique_ptr<PGetNextNode> QueryFactory(LAbstractNode* node) {
  if (dynamic_cast<LSelectNode*>(node) != nullptr) {
    LSelectNode* tmp = (LSelectNode*)node;
    std::vector<Predicate> p;
    return std::unique_ptr<PSelectNode>(new PSelectNode(tmp, p));
  } else if (dynamic_cast<LJoinNode*>(node) != nullptr) {
    LJoinNode *n = dynamic_cast<LJoinNode*>(node);
    if (n->type == LJoinType::NESTED_LOOP) {
      return std::unique_ptr<PNestedLoopJoinNode>(new PNestedLoopJoinNode(
        QueryFactory(node->GetLeft()),
        QueryFactory(node->GetRight()),
        node));
    } else if (n->type == LJoinType::SORTED_MERGE) {
      return std::unique_ptr<PSortedJoinNode>(new PSortedJoinNode(
        QueryFactory(node->GetLeft()),
        QueryFactory(node->GetRight()),
        node));
    } else if (n->type == LJoinType::HASH_JOIN) {
      return std::unique_ptr<PHashJoinNode>(new PHashJoinNode(
        QueryFactory(node->GetLeft()),
        QueryFactory(node->GetRight()),
        node));
    } else {
      assert(false);
      return nullptr;
    }
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
  } else if (dynamic_cast<LUnionNode*>(node) != nullptr) {
    return std::unique_ptr<PUnionNode>(new PUnionNode(
        QueryFactory(node->GetLeft()),
        QueryFactory(node->GetRight()),
        node));
  } else if (dynamic_cast<LSortNode*>(node) != nullptr) {
    return std::unique_ptr<PSortNode>(new PSortNode(
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

int main(int argc, char* argv[]) {
  if (argc >= 2) {
    PGetNextNode::BLOCK_SIZE = atoi(argv[1]);
  }
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
    std::unique_ptr<LJoinNode> n3(new LJoinNode(std::move(n1), std::move(n2), "table1.id", "table2.id2", 666, LJoinType::NESTED_LOOP));
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
    std::unique_ptr<LJoinNode> n3(new LJoinNode(std::move(n1), std::move(n2), "table1.id", "table2.id2", 666, LJoinType::NESTED_LOOP));
    std::unique_ptr<LProjectNode> n4(new LProjectNode(std::move(n3), {"table2.type2", "table1.description"}));
    std::unique_ptr<LUniqueNode> n5(new LUniqueNode(std::move(n4)));
    {
      std::unique_ptr<PGetNextNode> q1 = QueryFactory(n5.get());
      q1->Print(0);
      ExecuteQuery(q1.get());
    }

    std::cout << std::endl << "Query5: sort of Query4" << std::endl;
    std::unique_ptr<LSortNode> n6(new LSortNode(std::move(n5), "table2.type2"));
    {
      std::unique_ptr<PGetNextNode> q1 = QueryFactory(n6.get());
      q1->Print(0);
      ExecuteQuery(q1.get());
    }
  }
  {
    std::cout << std::endl << "Query6: same as Query4, but with sorted-join and with table1.id" << std::endl;
    BaseTable bt1 = BaseTable("table1");
    BaseTable bt2 = BaseTable("table2");
    std::cout << bt1;
    std::cout << bt2;
    std::unique_ptr<LAbstractNode> n1(new LSelectNode(bt1, {Predicate(PT_GREATERTHAN, VT_INT, 2, 30, "")}));
    std::unique_ptr<LAbstractNode> n2(new LSelectNode(bt2, {}));
    std::unique_ptr<LSortNode> n3(new LSortNode(std::move(n1), "table1.id"));
    std::unique_ptr<LSortNode> n4(new LSortNode(std::move(n2), "table2.id2"));
    std::unique_ptr<LJoinNode> n5(new LJoinNode(std::move(n3), std::move(n4), "table1.id", "table2.id2", 666, LJoinType::SORTED_MERGE));
    std::unique_ptr<LProjectNode> n6(new LProjectNode(std::move(n5), {"table1.id", "table2.type2", "table1.description"}));
    std::unique_ptr<LUniqueNode> n7(new LUniqueNode(std::move(n6)));
    std::unique_ptr<PGetNextNode> q1 = QueryFactory(n7.get());
    q1->Print(0);
    ExecuteQuery(q1.get());
  }
  {
    std::cout << std::endl << "Query7: same as Query4, but with hash-join" << std::endl;
    BaseTable bt1 = BaseTable("table1");
    BaseTable bt2 = BaseTable("table2");
    std::cout << bt1;
    std::cout << bt2;
    std::unique_ptr<LAbstractNode> n1(new LSelectNode(bt1, {Predicate(PT_GREATERTHAN, VT_INT, 2, 30, "")}));
    std::unique_ptr<LAbstractNode> n2(new LSelectNode(bt2, {}));
    std::unique_ptr<LJoinNode> n3(new LJoinNode(std::move(n1), std::move(n2), "table1.id", "table2.id2", 666, LJoinType::HASH_JOIN));
    std::unique_ptr<LProjectNode> n4(new LProjectNode(std::move(n3), {"table2.type2", "table1.description"}));
    std::unique_ptr<LUniqueNode> n5(new LUniqueNode(std::move(n4)));
    std::unique_ptr<PGetNextNode> q1 = QueryFactory(n5.get());
    q1->Print(0);
    ExecuteQuery(q1.get());
  }
  {
    std::cout << std::endl << "Query8: union three differently sorted tables" << std::endl;
    BaseTable bt1 = BaseTable("table1");
    std::cout << bt1;
    std::unique_ptr<LAbstractNode> n1(new LSelectNode(bt1, {}));
    std::unique_ptr<LAbstractNode> n2(new LSortNode(std::move(n1), "table1.description"));
    std::unique_ptr<LAbstractNode> n3(new LSelectNode(bt1, {}));
    std::unique_ptr<LAbstractNode> n4(new LSortNode(std::move(n3), "table1.frequency"));
    std::unique_ptr<LUnionNode> n5(new LUnionNode(std::move(n2), std::move(n4)));
    std::unique_ptr<PGetNextNode> q1 = QueryFactory(n5.get());
    q1->Print(0);
    ExecuteQuery(q1.get());
    q1->Print(0, true);
  }
  if (argc >= 3 && !strcmp(argv[2], "big")) {
    std::cout << std::endl << "Query9: with big1/big2 sorted-join and with table1.id" << std::endl;
    BaseTable bt1 = BaseTable("big1");
    BaseTable bt2 = BaseTable("big2");
    std::cout << bt1;
    std::cout << bt2;
    std::unique_ptr<LAbstractNode> n1(new LSelectNode(bt1, {}));
    std::unique_ptr<LAbstractNode> n2(new LSelectNode(bt2, {}));
    std::unique_ptr<LSortNode> n3(new LSortNode(std::move(n1), "big1.id"));
    std::unique_ptr<LSortNode> n4(new LSortNode(std::move(n2), "big2.id"));
    std::unique_ptr<LJoinNode> n5(new LJoinNode(std::move(n3), std::move(n4), "big1.id", "big2.id", 666, LJoinType::SORTED_MERGE));
    std::unique_ptr<PGetNextNode> q1 = QueryFactory(n5.get());
    q1->Print(0);
    ExecuteQuery(q1.get());
    q1->Print(0, true);
  }
}
