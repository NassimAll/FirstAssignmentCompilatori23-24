//===-- LocalOpts.cpp - Example Transformations --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/Utils/LocalOpts.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
#include <cmath>

using namespace llvm;

bool runOnBasicBlock(BasicBlock &B) {
    
    for (auto &I : B) {
        //Instruction *I = &II;
        // check if instruction is a binary operator instruction
        if(dyn_cast<BinaryOperator>(&I)) {
            // Algebraic Identity with add
            if (I.getOpcode() == Instruction::Add) {
                if(ConstantInt *C = dyn_cast<ConstantInt>(I.getOperand(0))) {
                    if (C->isZero()){
                        outs() << "Replacing a sum with 0 \n";
                        I.replaceAllUsesWith(I.getOperand(1));
                        continue;
                    }
                }

                if(ConstantInt *C = dyn_cast<ConstantInt>(I.getOperand(1))) {
                    if (C->isZero()){
                        outs() << "Replacing a sum with 0 \n";
                        I.replaceAllUsesWith(I.getOperand(0));
                        continue;
                    }
                }

            }
            // Algebraic Identity with mul
            if (I.getOpcode() == Instruction::Mul) {
                if(ConstantInt *C = dyn_cast<ConstantInt>(I.getOperand(0))) {
                    if (C->getValue() == 1){
                        outs() << "Replacing a mul with 1 \n";
                        I.replaceAllUsesWith(I.getOperand(1));
                        continue;
                    }
                }

                if(ConstantInt *C = dyn_cast<ConstantInt>(I.getOperand(1))) {
                    if (C->getValue() == 1){
                        outs() << "Replacing a mul with 1 \n";
                        I.replaceAllUsesWith(I.getOperand(0));
                        continue;
                    }
                }
            }
        // Strength Reduction of Multiply
        if (I.getOpcode() == Instruction::Mul) {

            if (ConstantInt *C = dyn_cast<ConstantInt>(I.getOperand(0))) {
                if (C->getValue().isPowerOf2()) {
                    ConstantInt *Cshl = ConstantInt::get(C->getType(), C->getValue().exactLogBase2());
                    Instruction *NewInst = BinaryOperator::Create(
                        Instruction::Shl, I.getOperand(1), Cshl);
                    outs() << "Creating a shift left of " << Cshl->getValue() << " \n";
                    NewInst->insertAfter(&I);
                    I.replaceAllUsesWith(NewInst);
                    continue;
                }
            }

            if (ConstantInt *C = dyn_cast<ConstantInt>(I.getOperand(1))) {
                if (C->getValue().isPowerOf2()) {
                    ConstantInt *Cshl = ConstantInt::get(C->getType(), C->getValue().exactLogBase2());
                    Instruction *NewInst = BinaryOperator::Create(
                        Instruction::Shl, I.getOperand(0), Cshl);
                    outs() << "Creating a shift left of " << Cshl->getValue() << " \n";
                    NewInst->insertAfter(&I);
                    I.replaceAllUsesWith(NewInst);
                    continue;
                }
            }
            }

            // Strength Reduction of division 
            if (I.getOpcode() == Instruction::SDiv) {
                if(ConstantInt *C = dyn_cast<ConstantInt>(I.getOperand(1))) {
                    if (C->getValue().isPowerOf2()){
                        ConstantInt *Crhl = ConstantInt::get(C->getType(), C->getValue().exactLogBase2());
                        outs() << "Replacing a sdvi with a right shift of " << Crhl->getValue() << " \n";
                        Instruction *NewI = BinaryOperator::Create(
                             Instruction::LShr, I.getOperand(0), Crhl);
                        NewI->insertAfter(&I);
                        I.replaceAllUsesWith(NewI);
                        continue;
                    }
                }
            }

        }

    }

      return true;
}



bool runOnFunction(Function &F) {
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    if (runOnBasicBlock(*Iter)) {
      Transformed = true;
    }
  }

  return Transformed;
}


PreservedAnalyses LocalOpts::run(Module &M,
                                      ModuleAnalysisManager &AM) {
  for (auto Fiter = M.begin(); Fiter != M.end(); ++Fiter)
    if (runOnFunction(*Fiter))
      return PreservedAnalyses::none();
  
  return PreservedAnalyses::all();
}

