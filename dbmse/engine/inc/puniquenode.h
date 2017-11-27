#ifndef PUNIQUENODE_H
#define PUNIQUENODE_H

#include <memory>
#include <vector>
#include "pgetnextnode.h"

class PUniqueNode : public PGetNextNode {
  public:
    PUniqueNode(std::unique_ptr<PGetNextNode> child, LAbstractNode* p);
    std::vector<std::vector<Value>> GetNext() override;
    void Rewind() override;
    void Print(int indent, bool print_stats) override;
  private:
    std::vector<std::vector<Value>> past;
};


#endif // PUNIQUENODE_H
