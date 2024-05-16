//===-- LocalOpts.cpp - Example Transformations --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/Utils/LoopWalk.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Dominators.h"
#include <cmath>

using namespace llvm;

typedef std::set<Instruction*> Invariants;

bool isLoopInvariantOperand(llvm::Value *Operand, Loop &L) {

  if (llvm::Instruction *op1 = dyn_cast<llvm::Instruction>(Operand)) {
    if (L.contains(op1->getParent())) {
      return false;
    }
  }

  // if(llvm::Instruction *Op = dyn_cast<llvm::Instruction>(Operand)) {
  //   outs () << *Op->getParent() << "\n";
  //   if(L.contains(Op->getParent())) return false;
  // }
  return true;
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

  // BasicBlock *head = loop.getHeader();
  // Function *F = head->getParent();
  auto loopBlocks = loop.getBlocks();
  for (auto &block : loopBlocks) {
      bool dominateExit = false;
      outs() << "--------------------------- BLOCK ----------------------------- \n";
      block->print(outs());
      outs() << "---------------------------  ----------------------------- \n\n";
      /*
        Prima di tutto si controlla se il blocco del loop in esame domina le uscite
      */
      dominateExit = DT.dominates(block, exitBlock);
      outs() << block.getName() << " - Domina le uscite: " << dominateExit << "\n";

      findBlockInvariants(loop,block)
  }

  

  return true;
}

void findBlockInvariants(Loop &loop, BasicBlock &block) {
  // for (auto &BB : *F) {
  //   for(auto &I : BB) {
  //     if (dyn_cast<BinaryOperator>(&I)) {
  //       //outs () << I << "\n";
  //       if (isLoopInvariantOperand(I.getOperand(0), L) && isLoopInvariantOperand(I.getOperand(1), L)) {
  //         outs() << "Inst : " << I << " have reaching def outside\n";
  //       } else {
  //         outs() << "Inst : " << I << " have reaching def Inside\n";
  //       }
  //     }
  //   } 
  // }
}

PreservedAnalyses LoopWalk::run(Loop &L, LoopAnalysisManager &LAM, LoopStandardAnalysisResults &LAR, LPMUpdater &LU) {

  if (!runOnLoop(L,LAM,LAR,LU)){
    return PreservedAnalyses::none();
  }
  

  // for (auto &BB : *F) {
  //   outs() << "BB : " << BB.getName() << "\n";
  //   if (BB.getName() == "body") {
  //     for(auto &I : BB) {
  //       outs() << "Inst : " << I << "\n";

  //       Value *firstOperand = I.getOperand(0);
  //       Value *secondOperand = I.getOperand(1);

  //       for (auto B : L.blocks()) {
          
  //       }
  //     }
  //   }

  return PreservedAnalyses::all();
}

