#ifndef PCROSSPRODUCTNODE_H
#define PCROSSPRODUCTNODE_H

#include "pgetnextnode.h"

class PCrossProductNode : public PGetNextNode {
  public:
    PCrossProductNode(std::unique_ptr<PGetNextNode> left, std::unique_ptr<PGetNextNode> right, LAbstractNode* p);
    std::vector<std::vector<Value>> GetNext() override;
    void Initialize();
    void Print(int indent) override;
};


#endif // PCROSSPRODUCTNODE_H
