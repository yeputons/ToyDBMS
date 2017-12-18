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

#include <typeinfo>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <tuple>
#include "interface.h"
#include "basics.h"
#include "pgetnextnode.h"
#include "engine.h"

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
      std::cout << "\n";
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
    std::unique_ptr<LAbstractNode> n1(new LSelectNode(bt1, &TruePredicate::INSTANCE));
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
    std::unique_ptr<LAbstractNode> n1(new LSelectNode(bt1, &TruePredicate::INSTANCE));
    std::unique_ptr<LAbstractNode> n2(new LSelectNode(bt2, &TruePredicate::INSTANCE));
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
    std::unique_ptr<LAbstractNode> n1(new LSelectNode(bt1, &TruePredicate::INSTANCE));
    std::unique_ptr<LAbstractNode> n2(new LSelectNode(bt2, &TruePredicate::INSTANCE));
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
    LeafPredicate p(PT_GREATERTHAN, 2, Value(30));
    std::unique_ptr<LAbstractNode> n1(new LSelectNode(bt1, &p));
    std::unique_ptr<LAbstractNode> n2(new LSelectNode(bt2, &TruePredicate::INSTANCE));
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
    LeafPredicate p(PT_GREATERTHAN, 2, Value(30));
    std::unique_ptr<LAbstractNode> n1(new LSelectNode(bt1, &p));
    std::unique_ptr<LAbstractNode> n2(new LSelectNode(bt2, &TruePredicate::INSTANCE));
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
    LeafPredicate p(PT_GREATERTHAN, 2, Value(30));
    std::unique_ptr<LAbstractNode> n1(new LSelectNode(bt1, &p));
    std::unique_ptr<LAbstractNode> n2(new LSelectNode(bt2, &TruePredicate::INSTANCE));
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
    std::unique_ptr<LAbstractNode> n1(new LSelectNode(bt1, &TruePredicate::INSTANCE));
    std::unique_ptr<LAbstractNode> n2(new LSortNode(std::move(n1), "table1.description"));
    std::unique_ptr<LAbstractNode> n3(new LSelectNode(bt1, &TruePredicate::INSTANCE));
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
    std::unique_ptr<LAbstractNode> n1(new LSelectNode(bt1, &TruePredicate::INSTANCE));
    std::unique_ptr<LAbstractNode> n2(new LSelectNode(bt2, &TruePredicate::INSTANCE));
    std::unique_ptr<LSortNode> n3(new LSortNode(std::move(n1), "big1.id"));
    std::unique_ptr<LSortNode> n4(new LSortNode(std::move(n2), "big2.id"));
    std::unique_ptr<LJoinNode> n5(new LJoinNode(std::move(n3), std::move(n4), "big1.id", "big2.id", 666, LJoinType::SORTED_MERGE));
    std::unique_ptr<PGetNextNode> q1 = QueryFactory(n5.get());
    q1->Print(0);
    ExecuteQuery(q1.get());
    q1->Print(0, true);
  }
  {
    std::cout << std::endl << "Query10: plain select with AND of predicates" << std::endl;
    BaseTable bt1 = BaseTable("table1");
    std::cout << bt1;
    LeafPredicate a(PT_GREATERTHAN, 2, 30);
    LeafPredicate b(PT_GREATERTHAN, 3, 40);
    AndPredicate c(&a, &b);
    std::unique_ptr<LAbstractNode> n1(new LSelectNode(bt1, &c));
    std::unique_ptr<PGetNextNode> q1 = QueryFactory(n1.get());
    q1->Print(0);
    ExecuteQuery(q1.get());
  }
  {
    std::cout << std::endl << "Query11: plain select with OR of predicates" << std::endl;
    BaseTable bt1 = BaseTable("table1");
    std::cout << bt1;
    LeafPredicate a(PT_GREATERTHAN, 2, 30);
    LeafPredicate b(PT_GREATERTHAN, 3, 40);
    OrPredicate c(&a, &b);
    std::unique_ptr<LAbstractNode> n1(new LSelectNode(bt1, &c));
    std::unique_ptr<PGetNextNode> q1 = QueryFactory(n1.get());
    q1->Print(0);
    ExecuteQuery(q1.get());
  }
  {
    std::cout << std::endl << "Query12: same as Query4, but with double-ended-hash-join" << std::endl;
    BaseTable bt1 = BaseTable("table1");
    BaseTable bt2 = BaseTable("table2");
    std::cout << bt1;
    std::cout << bt2;
    LeafPredicate p(PT_GREATERTHAN, 2, Value(30));
    std::unique_ptr<LAbstractNode> n1(new LSelectNode(bt1, &p));
    std::unique_ptr<LAbstractNode> n2(new LSelectNode(bt2, &TruePredicate::INSTANCE));
    std::unique_ptr<LJoinNode> n3(new LJoinNode(std::move(n1), std::move(n2), "table1.id", "table2.id2", 666, LJoinType::DE_HASH_JOIN));
    std::unique_ptr<LProjectNode> n4(new LProjectNode(std::move(n3), {"table2.type2", "table1.description"}));
    std::unique_ptr<LUniqueNode> n5(new LUniqueNode(std::move(n4)));
    std::unique_ptr<PGetNextNode> q1 = QueryFactory(n5.get());
    q1->Print(0);
    ExecuteQuery(q1.get());
  }
  if (argc >= 3 && !strcmp(argv[2], "big")) {
    std::cout << std::endl << "Query13: with big1/big2 double-ended-hash-join and with table1.id" << std::endl;
    BaseTable bt1 = BaseTable("big1");
    BaseTable bt2 = BaseTable("big2");
    std::cout << bt1;
    std::cout << bt2;
    std::unique_ptr<LAbstractNode> n1(new LSelectNode(bt1, &TruePredicate::INSTANCE));
    std::unique_ptr<LAbstractNode> n2(new LSelectNode(bt2, &TruePredicate::INSTANCE));
    std::unique_ptr<LSortNode> n3(new LSortNode(std::move(n1), "big1.id"));
    std::unique_ptr<LSortNode> n4(new LSortNode(std::move(n2), "big2.id"));
    std::unique_ptr<LJoinNode> n5(new LJoinNode(std::move(n3), std::move(n4), "big1.id", "big2.id", 666, LJoinType::DE_HASH_JOIN));
    std::unique_ptr<PGetNextNode> q1 = QueryFactory(n5.get());
    q1->Print(0);
    ExecuteQuery(q1.get());
    q1->Print(0, true);
  }
}
