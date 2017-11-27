#ifndef PPROJECTNODE_H
#define PPROJECTNODE_H

#include <memory>
#include <vector>
#include "pgetnextnode.h"

class PProjectNode : public PGetNextNode {
  public:
    PProjectNode(std::unique_ptr<PGetNextNode> child, LAbstractNode* p);
    std::vector<std::vector<Value>> GetNext() override;
    void Rewind() override;
    void Print(int indent) override;
  private:
    std::vector<std::vector<Value>> data;
    std::vector<int> indices;
};


#endif // PPROJECTNODE_H
