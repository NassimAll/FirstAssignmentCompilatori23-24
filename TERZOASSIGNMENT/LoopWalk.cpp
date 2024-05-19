
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

  // Facciamo il controllo su titti gli operandi dell'istruzione
  for(auto it = I->op_begin(); it != I->op_end(); ++it) {
    if (!isOperandInvariant(*it, loop)) 
      return false;
  }
  outs() << "This instruction can be removed: " << *I << "\n";
  
  return true;
}

bool isDead(Instruction *I, Loop &loop) {
  bool isDead = true;
  //Itero sugli usi e se tutti gli usi sono nel for allora l'istruzione si uò reputare dead
  for(auto u = I->use_begin(); u != I->use_end() && isDead; ++u) {
    Instruction *instr = dyn_cast<Instruction>(&*u);
    if(!loop.contains(instr))
        isDead = false;
  }
  outs() << "Instruction " << *I << " isDead: " << isDead << "\n\n";
  return isDead;
}
/*
  Controllo che le istruzioni siano loop invariant
*/
void findInstInvariants(Loop &loop, BasicBlock &block) {
  for(auto &I : block) {
    if (isInstructionLoopInvariant(&I, loop)) {
        Invariants.insert(&I);
        InstToMove.push_back(&I);
      }
  } 
}
/*
  Controllo che le istruzioni siano sia dead che loop invariant
*/
void findInstInvariantsDead(Loop &loop, BasicBlock &block) {
  for(auto &I : block) {
    if (isInstructionLoopInvariant(&I, loop) && isDead(&I, loop)) {
        Invariants.insert(&I);
        InstToMove.push_back(&I);
      }
  } 
}


bool runOnLoop(Loop &loop, LoopAnalysisManager &LAM, LoopStandardAnalysisResults &LAR, LPMUpdater &LU) {

  bool analysisDone = false;
   // check if there is a preheader
  BasicBlock* preheader = loop.getLoopPreheader();
  outs() << "--------------------------- PRE-HEADER ----------------------------- \n";
  preheader->print(outs());
  outs() << "---------------------------  ----------------------------- \n\n";
  if (!preheader) {
    return false;
  }
  // Carico nel vettore l'uscita/le uscite del nostro loop 
  SmallVector<BasicBlock*> vec {};
  loop.getExitBlocks(vec);
 
  outs() << "--------------------------- EXIT BLOCK ----------------------------- \n";
  for(auto it = vec.begin(); it != vec.end(); ++it) {
    BasicBlock *exitBlock = *it;
    exitBlock->print(outs());
    outs() << "\n\n";
  }
  outs() << "---------------------------  ----------------------------- \n\n";

  //Ottengo il dominator tree e lo stampo
  llvm::DominatorTree &DT = LAR.DT;
  outs() << "--------------------------- DOMINATOR TREE ----------------------------- \n";
  DT.print(outs());
  outs() << "\n\n";

  //Semplice stampa del blocco root, non serve per la licm
  BasicBlock *BB = (DT.getRootNode())->getBlock();
  outs() << "--------------------------- ROOT BLOCK ----------------------------- \n";
  BB->print(outs());
  outs() << "---------------------------  ----------------------------- \n\n";

  //Iteriamo sui blocchi del loop per controllarli tutti singolarmente
  auto loopBlocks = loop.getBlocks();
  for (auto &block : loopBlocks) {
      bool dominateExits = true;
      outs() << "--------------------------- BLOCK ----------------------------- \n";
      block->print(outs());
      outs() << "\n\n";
      /*
        Prima di tutto si controlla se il blocco del loop in esame domina tutte le uscite
      */
      for(auto it = vec.begin(); it != vec.end(); ++it) {
          BasicBlock *exitBlock = *it;
          if(!DT.dominates(block, exitBlock))
              dominateExits = false;
      }
      
      outs() << block->getName() << " - Domina le uscite: " << dominateExits << "\n";
      /*
        Se il blocco domina tutte le uscite cerchiamo al suo interlo le istruzioni loop invariant
        invece se il blocco non domina tutte le uscite controlliamo che le istruzioni siano 
        loop invariant e siano morte alla fine del ciclo (ovvero non abbiano usi )
      */
      if (dominateExits) 
        findInstInvariants(loop, *block);
      else
        findInstInvariantsDead(loop, *block);
  }

  // Iteriamo sul vettore che contiene le istruzioni da spostare e le spostiamo
  for (auto &I : InstToMove) {
    outs () << "Inst to move: " << *I << "\n";
    I->moveBefore(preheader->getTerminator());
    analysisDone = true;
  }

  outs() << "--------------------------- NEW PRE-HEADER ----------------------------- \n";
  preheader->print(outs());
  outs() << "---------------------------   ----------------------------- \n\n";

  return analysisDone;
}


PreservedAnalyses LoopWalk::run(Loop &L, LoopAnalysisManager &LAM, LoopStandardAnalysisResults &LAR, LPMUpdater &LU) {

  if (!runOnLoop(L,LAM,LAR,LU)){
    return PreservedAnalyses::none();
  }
  
  return PreservedAnalyses::all();
}

