#include "engine.h"
#include "pselectnode.h"
#include "pnestedloopjoinnode.h"
#include "psortedjoinnode.h"
#include "phashjoinnode.h"
#include "pcrossproductnode.h"
#include "pprojectnode.h"
#include "puniquenode.h"
#include "punionnode.h"
#include "psortnode.h"

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
