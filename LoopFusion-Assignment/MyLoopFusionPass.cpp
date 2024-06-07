
#include "llvm/Transforms/Utils/MyLoopFusionPass.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/DependenceAnalysis.h"

using namespace llvm;

bool isAdjacent(Loop *L0, Loop *L1, bool &isG) {
  outs() << L0->getLoopGuardBranch() << "\n";
  outs() << L1->getLoopPreheader() << "\n";
  if (L0->isGuarded() && L1->isGuarded()) {
    outs() << "Entrambi guarded" << "\n";

    if (L0->getExitBlock()->getSingleSuccessor() == L1->getLoopGuardBranch()->getParent()) {
      outs() << "Branch guard L0 uguale a entry successiva L1" << "\n";
      isG = true;

      //CHECK SE HANNO LA STESSA GUARDIA PERCHE ALTRIMWNTI IMPOSSIBILE FONDERE
      if (auto L0CmpInst = dyn_cast<Instruction>(L0->getLoopGuardBranch()->getCondition()))
        if (auto L1CmpInst = dyn_cast<Instruction>(L1->getLoopGuardBranch()->getCondition()))
          if (!L0CmpInst->isIdenticalTo(L1CmpInst))
            return false;
          else {
            outs() << "guardie identiche" << "\n";
            return true;
          }
      
    }else return false;

  } else if (!L0->isGuarded() && L1->isGuarded() || L0->isGuarded() && !L1->isGuarded()) {
    outs() << "Uno guarded e l'altro no, fusion impossibile" << "\n";
    return false;
  }else {
    outs() << "Non sono entrambi guarded" << "\n";
    if (L0->getUniqueExitBlock() == L1->getLoopPreheader()) {
      outs() << "Uscita L0 uguale a preheader L1, adiacenti" << "\n";
      return true;
    } else return false;
  }
}

bool isControlFlowEquivalent(DominatorTree &DT, PostDominatorTree &PDT, Loop *L0, Loop *L1, bool isG) {  
  // DT.print(outs());
  // outs() << "\n\n";
  // PDT.print(outs());
  // outs() << "\n\n";
  if (isG) {
    if(DT.dominates(L0->getLoopGuardBranch()->getParent(),L1->getLoopGuardBranch()->getParent())) {
      outs() << "L0 domina L1 \n";
      if (PDT.dominates(L1->getLoopGuardBranch()->getParent(),L0->getLoopGuardBranch()->getParent())) {
        outs() << "L1 post-domina L0 \n";
        return true;
      }else return false;
    }else return false;
  } else {
    if(DT.dominates(L0->getUniqueExitBlock(),L1->getLoopPreheader())) {
      outs() << "L0 domina L1 \n";
      if (PDT.dominates(L1->getLoopPreheader(),L0->getUniqueExitBlock())) {
        outs() << "L1 post-domina L0 \n";
        return true;
      }else return false;
    }else return false;
  }

}

//Controllo che i loop abbiano lo stesso trip count
bool sameIterate(ScalarEvolution &SE, Loop *L0, Loop *L1) {
  auto CountL0 = SE.getBackedgeTakenCount (L0);
  auto CountL1 = SE.getBackedgeTakenCount (L1);
  // auto c0 = SE.getSmallConstantTripCount(L0);
  // auto c1 = SE.getSmallConstantTripCount(L1);
  outs() << "CL0: " << CountL0 << " CL1: " << CountL1 << " \n";
  //outs() << "CL0: " << c0 << " CL1: " << c1 << " \n";

  if (CountL0->getSCEVType() == SCEVCouldNotCompute().getSCEVType() || CountL1->getSCEVType() == SCEVCouldNotCompute().getSCEVType()) return false;

  if (CountL0 == CountL1) {
    outs() << "Stesso iteration trip count. \n";
    return true;
  }
  return false;
}

bool haveDependecies(Loop *L0, Loop *L1) {
  auto BlocksL0 = L0->getBlocks();
  bool dep = false;
  for (auto &BB : BlocksL0) {
    for (Instruction &I : *BB) {
      // Find instructions that works on arrays and get the array pointer
      if (I.getOpcode() == Instruction::GetElementPtr){
        
        outs() << "\nInstruction: " << I << "\n";
        outs() << "Use in second loop:\n";
        // Check if pointer is used in the second loop
        for (auto &use : I.getOperand(0)->uses()) {
          if (auto inst = dyn_cast<Instruction>(use.getUser())) {
            if (L1->contains(inst)) {
              outs() << *inst << "\n\n";
               
              // Check if the instruction where the pointer is used, uses a sext instruction. This instuction should be another GetElementPtr in L2
              if (auto sext = dyn_cast<Instruction>(inst->getOperand(1))) {
                outs() << "sext " <<  *sext << "\n\n";
                // Check if the sext instruction uses a PHI instruction, if it doesn't it means that there is another instruction that alters the value of the offset
                if (auto phyInstruction = dyn_cast<Instruction>(sext->getOperand(0))) {
                  switch(phyInstruction->getOpcode()) {
                    case Instruction::PHI:
                    case Instruction::Sub: // If the non-phi instruction is a sub, it means I'm working with a negative offset, in this case you should still be able to merge the loops.
                      break;
                    default:{
                      outs() << *phyInstruction << "\n\n";
                      dep = true; 
                      //outs() << "Dipendenza: " << dep << "\n\n";
                      break;
                    }  
                 }
                }
              }
            }
          }
        }
      } 
    }
  }
  outs() << "Dipendenza: " << dep << "\n\n";
  return dep;
}

//La fusione di due loop garded è differente perchè bisogna tenere conto delle guardie nella fusione
// NOTA: Si possono fondere solo i loop che hanno la guardia identica 
void fusionGarded(Loop *L0, Loop *L1) {
  // Replace uses of IV in L2 with IV in L1
  outs() << " GARDED FUSION \n";

  auto inductionL0 = L0->getCanonicalInductionVariable();
  auto inductionL1 = L1->getCanonicalInductionVariable();

  outs() << "First loop IV: " << *inductionL0 << "\n";
  outs() << "Second loop IV: " << *inductionL1 << "\n";

  inductionL1->replaceAllUsesWith(inductionL0);
  
  auto header1 = L0->getHeader();
  auto header2 = L1->getHeader();

  auto latch1 = L0->getLoopLatch();
  auto latch2 = L1->getLoopLatch();

  auto exit = L1->getUniqueExitBlock();

  // guard1 --> L2 exit
  // latch1 --> L2 exit
  // header1 --> header2
  // header2 --> latch1

  auto guard1 = L0->getLoopGuardBranch()->getParent();
  auto guard2 = L1->getLoopGuardBranch()->getParent();

  // Attach guard 1 to L2 exit
  BranchInst::Create(L0->getLoopPreheader(), exit, guard1->back().getOperand(0), guard1->getTerminator());
  guard1->getTerminator()->eraseFromParent();

  // Attach latch 1 to L2 exit
  BranchInst::Create(L0->getBlocks().front(), exit, latch1->back().getOperand(0), latch1->getTerminator());
  latch1->getTerminator()->eraseFromParent();

  // Attach header 1 to header 2
  L0->getBlocks().drop_back(1).back()->getTerminator()->setSuccessor(0, L1->getBlocks().front());

  // Attach header 2 to latch 1
  L1->getBlocks().drop_back(1).back()->getTerminator()->setSuccessor(0, latch1);

  // Remove header 2 PHI node
  header2->front().eraseFromParent();

}

void fusion(Loop *L0, Loop *L1) {
  // Replace uses of IV in L2 with IV in L1
  outs() << " NORMAL FUSION \n";

  auto inductionL0 = L0->getCanonicalInductionVariable();
  auto inductionL1 = L1->getCanonicalInductionVariable();

  outs() << "First loop IV: " << *inductionL0 << "\n";
  outs() << "Second loop IV: " << *inductionL1 << "\n";

  inductionL1->replaceAllUsesWith(inductionL0);

  // Modify CFG as follows:
  // header 1 --> L2 exit
  // body 1 --> body 2
  // body 2 --> latch 1
  // header 2 --> latch 2
  
  auto header1 = L0->getHeader();
  auto header2 = L1->getHeader();

  auto latch1 = L0->getLoopLatch();
  auto latch2 = L1->getLoopLatch();

  auto exit = L1->getUniqueExitBlock();

  /* llvm::outs() << "Header 1: " << *header1 << "\n";
  llvm::outs() << "Header 2: " << *header2 << "\n";
  llvm::outs() << "Latch 1: " << *latch1 << "\n";
  llvm::outs() << "Latch 2: " << *latch2 << "\n";*/ 

  BasicBlock* lastL1BodyBB = L0->getBlocks().drop_back(1).back();
  
  outs() << "Block list of L2: ";
  for (auto &BB : L1->getBlocks().drop_front(1).drop_back(1)) {
    llvm::outs() << *BB << "\n";
  }

  // Attach body 2 to body 1
  lastL1BodyBB->getTerminator()->setSuccessor(0, L1->getBlocks().drop_front(1).drop_back(1).front());

  // Attach latch 1 to body 2
  L1->getBlocks().drop_front(1).drop_back(1).back()->getTerminator()->setSuccessor(0, latch1);

  // Attach header 2 to latch 2
  BranchInst::Create(latch2, header2->getTerminator());
  header2->getTerminator()->eraseFromParent();

  // Attach header 1 to L2 exit
  BranchInst::Create(L0->getBlocks().drop_front(1).front(), exit, header1->back().getOperand(0), header1->getTerminator());
  header1->getTerminator()->eraseFromParent();

}


PreservedAnalyses MyLoopFusionPass::run(Function &F, FunctionAnalysisManager &AM) {
  
  LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
  DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
  PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);
  ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F);
  DependenceInfo &DI = AM.getResult<DependenceAnalysis>(F);
  SmallVector<Loop*> loops {};
  bool isguard;
  bool analysisDone = false;

  loops = LI.getLoopsInPreorder();
  outs() << loops.size() << "\n";
  if (loops.size() < 2 ){
    outs() << "Non ci sono almeno 2 loop quindi impossibile eseguire la fusion" << "\n";
    return PreservedAnalyses::all();
  }  

  for(auto itL0 = loops.begin(); itL0 != loops.end(); ++itL0) {
    auto itL1 = itL0;
    Loop* L0 = *itL0;
    outs() << *L0 << "\n";
    for(++itL1; itL1 != loops.end(); ++itL1) {
      isguard = false;
      Loop* L1 = *itL1;
      outs() << *L1 << "\n";
      //PUNTO 1
      if (!isAdjacent(L0, L1, isguard))
        continue;
      outs() << isguard << "\n";
      //PUNTO 3
      if (!isControlFlowEquivalent(DT, PDT, L0, L1, isguard))
        continue;
      
      //PUNTO 2
      if (!sameIterate(SE, L0, L1))
        continue;

      //PUNTO 4
      if (haveDependecies(L0, L1))
        continue;
      if (!isguard)   fusion(L0,L1);
      else fusionGarded(L0,L1);

      analysisDone = true;
      break;      
    }
  }
  
  return (analysisDone ? PreservedAnalyses::none() : PreservedAnalyses::all());
}
