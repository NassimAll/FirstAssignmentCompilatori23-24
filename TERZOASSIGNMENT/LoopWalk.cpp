#include "llvm/Transforms/Utils/LoopWalk.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/ValueTracking.h"
#include <cmath>

using namespace llvm;

std::set<Instruction*> Invariants;
std::vector<Instruction*> InstToMove;

/**
 * @brief 
 *  
 *  Controlliamo che l'operando sia loop invariant, per esserelo può:
 *  - essere una costante
 *  - avere reaching def fuori dal loop 
 *  - derivare già da un istruzione loop invariant
 * 
 * @param operand Operando da controllare
 * @param loop Loop in esame
 * @return true 
 * @return false 
 */
bool isOperandInvariant(Value *operand, Loop &loop) {
  if (isa<llvm::Constant>(operand) || isa<llvm::Argument>(operand)) {
    return true;
  }

  if (llvm::Instruction *inst = dyn_cast<llvm::Instruction>(operand)) {
    if (!loop.contains(inst->getParent()) || Invariants.count(inst)) {
      return true;
    }
  }
  return false;
}

bool isInstructionLoopInvariant(Instruction *I, Loop &loop) {
  //Controllo che la nostra istruzione non abbia efetti collaterali e possa essere spostata
  if (!isSafeToSpeculativelyExecute(I)) {
    outs() << *I << " - Istruction is not safe to move \n\n";
    return false;
  }
  //opInvariant = true;
  for(auto it = I->op_begin(); it != I->op_end(); ++it) {
    if (!isOperandInvariant(*it, loop)) 
      return false;
  }
  outs() << "This instruction can be removed: " << *I << "\n";
  
  return true;
}

void findInstInvariants(Loop &loop, BasicBlock &block) {
  for(auto &I : block) {
    if (isInstructionLoopInvariant(&I, loop)) {
        Invariants.insert(&I);
        InstToMove.push_back(&I);
      }
  } 
}

bool runOnLoop(Loop &loop, LoopAnalysisManager &LAM, LoopStandardAnalysisResults &LAR, LPMUpdater &LU) {

   // check if there is a preheader
  BasicBlock* preheader = loop.getLoopPreheader();
  outs() << "--------------------------- PRE-HEADER ----------------------------- \n";
  preheader->print(outs());
  outs() << "---------------------------  ----------------------------- \n\n";
  if (!preheader) {
    return false;
  }
  // Creiamo un vettore che contiene tutte le possibili uscite del loop
  SmallVector<BasicBlock*> vec {};
  loop.getExitBlocks(vec);
 
  BasicBlock * exitBlock = loop.getUniqueExitBlock();
  outs() << "--------------------------- EXIT BLOCK ----------------------------- \n";
  exitBlock->print(outs());
  outs() << "---------------------------  ----------------------------- \n\n";


  llvm::DominatorTree &DT = LAR.DT;
  outs() << "--------------------------- DOMINATOR TREE ----------------------------- \n";
  DT.print(outs());
  outs() << "---------------------------  ----------------------------- \n\n";


  BasicBlock *BB = (DT.getRootNode())->getBlock();
  outs() << "--------------------------- ROOT BLOCK ----------------------------- \n";
  BB->print(outs());
  outs() << "---------------------------  ----------------------------- \n\n";

  /*
    Prima di tutto si controlla se il blocco del loop in esame domina tutte le uscite
    del loop
  */
  auto loopBlocks = loop.getBlocks();
  for (auto &block : loopBlocks) {
      bool dominateExits = true;
      outs() << "--------------------------- BLOCK ----------------------------- \n";
      block->print(outs());
      outs() << "---------------------------  ----------------------------- \n\n";

      // dominateExit = DT.dominates(block, exitBlock);
      // outs() << block->getName() << " - Domina le uscite: " << dominateExit << "\n";

      for(auto it = vec.begin(); it != vec.end(); ++it) {
          BasicBlock *exitBlock = *it;
          outs() << "BB Exit: " << exitBlock->getName() << "\n\n";
          if(!DT.dominates(block, exitBlock))
              dominateExits = false;
      }
      
      outs() << block->getName() << " - Domina le uscite: " << dominateExits << "\n";
      /*
        se il blocco domina tutte le uscite controlliamo se al suo interno sono 
        presenti delle istruzioni loop invariant
      */
      if (dominateExits) 
        findInstInvariants(loop, *block);
  }
  //Spostiamo  le istruzioni loop invariant
  for (auto &I : InstToMove) {
    outs () << "Inst to move: " << *I << "\n";
    I->moveBefore(preheader->getTerminator());
  }

  outs() << "--------------------------- NEW PRE-HEADER ----------------------------- \n";
  preheader->print(outs());
  outs() << "---------------------------   ----------------------------- \n\n";


  return true;
}


PreservedAnalyses LoopWalk::run(Loop &L, LoopAnalysisManager &LAM, LoopStandardAnalysisResults &LAR, LPMUpdater &LU) {

  if (!runOnLoop(L,LAM,LAR,LU)){
    return PreservedAnalyses::none();
  }
  
  return PreservedAnalyses::all();
}

