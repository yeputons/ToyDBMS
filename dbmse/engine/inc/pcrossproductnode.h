#ifndef PCROSSPRODUCTNODE_H
#define PCROSSPRODUCTNODE_H

#include "pgetnextnode.h"

class PCrossProductNode : public PGetNextNode {
  public:
    PCrossProductNode(std::unique_ptr<PGetNextNode> left, std::unique_ptr<PGetNextNode> right, LAbstractNode* p);
    std::vector<std::vector<Value>> GetNext() override;
    void Rewind() override;
    void Print(int indent, bool print_stats) override;
  private:
    std::vector<std::vector<Value>> lres, rres;
    int lptr;
};


#endif // PCROSSPRODUCTNODE_H
