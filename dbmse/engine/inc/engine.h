#ifndef ENGINE_H_
#define ENGINE_H_

#include <memory>
#include "interface.h"
#include "pgetnextnode.h"

std::unique_ptr<PGetNextNode> QueryFactory(LAbstractNode* node);

#endif  // ENGINE_H_
