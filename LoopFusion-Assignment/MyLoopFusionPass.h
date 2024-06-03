
#ifndef LLVM_TRANSFORMS_LOOPFUSIONPASS_H
#define LLVM_TRANSFORMS_LOOPFUSIONPASS_H
#include "llvm/IR/PassManager.h"
#include "llvm/Transforms/Scalar/LoopPassManager.h"


namespace llvm {

class MyLoopFusionPass : public PassInfoMixin<MyLoopFusionPass> {
public:
	PreservedAnalyses run(Function &F,
                                    FunctionAnalysisManager &AM);
};

} // namespace llvm

#endif 
