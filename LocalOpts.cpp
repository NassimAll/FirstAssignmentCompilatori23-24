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
        // check if instruction is a binary operator instruction
      if(dyn_cast<BinaryOperator>(&I)) {
            // Algebraic Identity with add
            if (I.getOpcode() == Instruction::Add) {
              //Check the first op 
                if(ConstantInt *C = dyn_cast<ConstantInt>(I.getOperand(0))) {
                    if (C->isZero()){
                        outs() << "Replacing a sum with 0 \n";
                        I.replaceAllUsesWith(I.getOperand(1));
                        continue;
                    }
                }
                //Check the second op 
                if(ConstantInt *C = dyn_cast<ConstantInt>(I.getOperand(1))) {
                    if (C->isZero()){
                        outs() << "Replacing a sum with 0 \n";
                        I.replaceAllUsesWith(I.getOperand(0));
                        continue;
                    }
                }

            }
           // Check if it's a mul
            if (I.getOpcode() == Instruction::Mul) {
               // Algebraic Identity with mul
               //check first op
                if(ConstantInt *C = dyn_cast<ConstantInt>(I.getOperand(0))) {
                    if (C->getValue() == 1){
                        outs() << "Replacing a mul with 1 \n";
                        I.replaceAllUsesWith(I.getOperand(1));
                        continue;
                    }
                }
                //check second op
                if(ConstantInt *C = dyn_cast<ConstantInt>(I.getOperand(1))) {
                    if (C->getValue() == 1){
                        outs() << "Replacing a mul with 1 \n";
                        I.replaceAllUsesWith(I.getOperand(0));
                        continue;
                    }
                }

            // Strength Reduction of Multiply
            //Check if the first op che be reduced
            if (ConstantInt *C = dyn_cast<ConstantInt>(I.getOperand(0))) {
                //check if it's a power of 2
                if (C->getValue().isPowerOf2()) {
                    ConstantInt *Cshl = ConstantInt::get(C->getType(), C->getValue().exactLogBase2());
                    Instruction *NewInst = BinaryOperator::Create(
                        Instruction::Shl, I.getOperand(1), Cshl);
                    outs() << "Creating a shift left of " << Cshl->getValue() << " \n";
                    NewInst->insertAfter(&I);
                    I.replaceAllUsesWith(NewInst);
                    continue;
                }else {
                  //Check if the first op can be reduced because is near a power of 2
                  APInt value = C->getValue();
                    if ((value-1).isPowerOf2()) {
                        ConstantInt *Cshl = ConstantInt::get(C->getType(), (value-1).exactLogBase2());
                        // Create the mul 
                        Instruction *mul = BinaryOperator::Create(
                            Instruction::Shl, I.getOperand(1), Cshl);
                        // complete the mul with an addition 
                        Instruction *NewInst = BinaryOperator::Create(
                            Instruction::Add, I.getOperand(1), mul);
                        outs() << "Creating a shift left of " << Cshl->getValue() << " and an add \n";
                        mul->insertAfter(&I);
                        NewInst->insertAfter(mul);
                        I.replaceAllUsesWith(NewInst);
                        continue;
                    }

                    if ((value+1).isPowerOf2()) {
                        ConstantInt *Cshl = ConstantInt::get(C->getType(), (value+1).exactLogBase2());
                        // Create the mul 
                        Instruction *mul = BinaryOperator::Create(
                            Instruction::Shl, I.getOperand(1), Cshl);
                        // complete the mul with a sub 
                        Instruction *NewInst = BinaryOperator::Create(
                            Instruction::Sub, mul, I.getOperand(1));
                        outs() << "Creating a shift left of " << Cshl->getValue() << " and a sub of " << C->getValue() <<" \n";
                        mul->insertAfter(&I);
                        NewInst->insertAfter(mul);
                        I.replaceAllUsesWith(NewInst);              
                        continue;
                    }
                }
              } 
            //Check if the second op che be reduced
            if (ConstantInt *C = dyn_cast<ConstantInt>(I.getOperand(1))) {
                if (C->getValue().isPowerOf2()) {
                    ConstantInt *Cshl = ConstantInt::get(C->getType(), C->getValue().exactLogBase2());
                    Instruction *NewInst = BinaryOperator::Create(
                        Instruction::Shl, I.getOperand(0), Cshl);
                    outs() << "Creating a shift left of " << Cshl->getValue() << " \n";
                    NewInst->insertAfter(&I);
                    I.replaceAllUsesWith(NewInst);
                    continue;
                } else {
                  //Check if the second op che be reduced because is near a power of 2
                    APInt value = C->getValue();
                    if ((value-1).isPowerOf2()) {
                        ConstantInt *Cshl = ConstantInt::get(C->getType(), (value-1).exactLogBase2());
                        // Create the mul 
                        Instruction *mul = BinaryOperator::Create(
                            Instruction::Shl, I.getOperand(0), Cshl);
                        // complete the mul with an addition 
                        Instruction *NewInst = BinaryOperator::Create(
                            Instruction::Add, I.getOperand(0), mul);
                        outs() << "Creating a shift left of " << Cshl->getValue() << " and an add \n";
                        mul->insertAfter(&I);
                        NewInst->insertAfter(mul);
                        I.replaceAllUsesWith(NewInst);
                    
                        continue;
                    }
                    //Check if the second op che be reduced because is near a power of 2
                    if ((value+1).isPowerOf2()) {
                        ConstantInt *Cshl = ConstantInt::get(C->getType(), (value+1).exactLogBase2());
                        // Create the mul 
                        Instruction *mul = BinaryOperator::Create(
                            Instruction::Shl, I.getOperand(0), Cshl);
                        // complete the mul with a sub 
                        Instruction *NewInst = BinaryOperator::Create(
                            Instruction::Sub, mul, I.getOperand(0));
                        outs() << "Creating a shift left of " << Cshl->getValue() << " and a sub of " << C->getValue() <<" \n";
                        mul->insertAfter(&I);
                        NewInst->insertAfter(mul);
                        I.replaceAllUsesWith(NewInst);
                    
                        continue;
                    }
                }
            }
            }

            // Strength Reduction of division 
            if (I.getOpcode() == Instruction::SDiv) {
              //check if the divider is a constant and a power of 2
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
    // Iterate to perform Multi-instruction optimization 
    for (auto &I : B) {
        if(dyn_cast<BinaryOperator>(&I)) {
            //check if the first op is an add 
            if (I.getOpcode() == Instruction::Add) {
                if (ConstantInt *C = dyn_cast<ConstantInt>(I.getOperand(1))){
                    //Iterare the user to find a sub that can be optimized
                    for (auto U = I.user_begin(); U != I.user_end(); ++U) {
                        Instruction *nextI = dyn_cast<Instruction>(*U);
                        if(nextI->getOpcode() == Instruction::Sub) {
                            if (C->getValue() == dyn_cast<ConstantInt>(nextI->getOperand(1))->getValue()){
                                nextI->replaceAllUsesWith(I.getOperand(0));
                                outs() << "Code optimization removing sub after add\n";
                                continue;
                            }
                        }
                    }
                }

                if (ConstantInt *C = dyn_cast<ConstantInt>(I.getOperand(0))){
                   for (auto U = I.user_begin(); U != I.user_end(); ++U) {
                        Instruction *nextI = dyn_cast<Instruction>(*U);
                        if(nextI->getOpcode() == Instruction::Sub) {
                            if (C->getValue() == dyn_cast<ConstantInt>(nextI->getOperand(1))->getValue()){
                                nextI->replaceAllUsesWith(I.getOperand(1));
                                outs() << "Code optimization removing sub after add\n";
                                continue;
                            }
                        }
                    }
                }
            }
            //Multi instruction optimized with first a sub
            if (I.getOpcode() == Instruction::Sub) {
                if (ConstantInt *C = dyn_cast<ConstantInt>(I.getOperand(1))){
                    for (auto U = I.user_begin(); U != I.user_end(); ++U) {
                        Instruction *nextI = dyn_cast<Instruction>(*U);
                        if(nextI->getOpcode() == Instruction::Add) {
                            if (C->getValue() == dyn_cast<ConstantInt>(nextI->getOperand(1))->getValue()){
                                nextI->replaceAllUsesWith(I.getOperand(0));
                                outs() << "Code optimization removing add after sub\n";
                                continue;
                            }else if (C->getValue() == dyn_cast<ConstantInt>(nextI->getOperand(0))->getValue()) {
                                nextI->replaceAllUsesWith(I.getOperand(0));
                                outs() << "Code optimization removing add after sub\n";
                                continue;
                            }
                        }
                    }
                }
            }
            //Multi optimization with mul
            if (I.getOpcode() == Instruction::Mul){
                if (ConstantInt *C = dyn_cast<ConstantInt>(I.getOperand(1))){
                    for (auto U = I.user_begin(); U != I.user_end(); ++U) {
                        Instruction *nextI = dyn_cast<Instruction>(*U);
                        if(nextI->getOpcode() == Instruction::SDiv) {
                            if (C->getValue() == dyn_cast<ConstantInt>(nextI->getOperand(1))->getValue()){
                                nextI->replaceAllUsesWith(I.getOperand(0));
                                outs() << "Code optimization mul/div removing the div \n";
                                continue;
                            }
                        }
                    }
                }

                if (ConstantInt *C = dyn_cast<ConstantInt>(I.getOperand(0))){
                   for (auto U = I.user_begin(); U != I.user_end(); ++U) {
                        Instruction *nextI = dyn_cast<Instruction>(*U);
                        if(nextI->getOpcode() == Instruction::SDiv) {
                            if (C->getValue() == dyn_cast<ConstantInt>(nextI->getOperand(1))->getValue()){
                                nextI->replaceAllUsesWith(I.getOperand(1));
                                outs() << "Code optimization mul/div removing the div \n";
                                continue;
                            }
                        }
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

