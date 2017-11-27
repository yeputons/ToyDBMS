#ifndef PSORTNODE_H
#define PSORTNODE_H

#include <memory>
#include <vector>
#include "pgetnextnode.h"

class PSortNode : public PGetNextNode {
  public:
    PSortNode(std::unique_ptr<PGetNextNode> child, LAbstractNode* p);
    std::vector<std::vector<Value>> GetNext() override;
    void Rewind() override;
    void Print(int indent) override;
  private:
    std::vector<std::vector<Value>> data;
};


#endif // PSORTNODE_H
