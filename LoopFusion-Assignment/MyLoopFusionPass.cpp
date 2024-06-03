
#include "llvm/Transforms/Utils/LoopWalk.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/LoopInfo.h"
#include <cmath>

using namespace llvm;

PreservedAnalyses MyLoopFusionPass::run(Function &F, FunctionAnalysisManager &AM) {
  
  LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
  

  return PreservedAnalyses::all();
}
