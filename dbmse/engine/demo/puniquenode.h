#ifndef PUNIQUENODE_H
#define PUNIQUENODE_H

#include <memory>
#include <vector>
#include "pgetnextnode.h"

class PUniqueNode : public PGetNextNode {
  public:
    PUniqueNode(std::unique_ptr<PGetNextNode> child, LAbstractNode* p);
    std::vector<std::vector<Value>> GetNext() override;
    void Initialize();
    void Print(int indent) override;
  private:
    std::vector<std::vector<Value>> data;
};


#endif // PUNIQUENODE_H
