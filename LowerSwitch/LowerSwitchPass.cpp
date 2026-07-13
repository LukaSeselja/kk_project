#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include <vector>

using namespace llvm;

namespace {
  struct LowerSwitchPass : public FunctionPass {
    static char ID;
    LowerSwitchPass() : FunctionPass(ID) {}

    bool processSwitchInst(SwitchInst *SI) {
      BasicBlock *CurBlock = SI->getParent();
      Function *F = CurBlock->getParent();
      Value *Cond = SI->getCondition();
      BasicBlock *DefaultBB = SI->getDefaultDest();

      std::vector<std::pair<ConstantInt *, BasicBlock *>> Cases;
      for (auto CaseIt : SI->cases())
        Cases.emplace_back(CaseIt.getCaseValue(), CaseIt.getCaseSuccessor());

      SI->eraseFromParent();

      if (Cases.empty()) {
        BranchInst::Create(DefaultBB, CurBlock);
        return true;
      }

      BasicBlock *CurrentBB = CurBlock;
      for (size_t i = 0; i < Cases.size(); ++i) {
        bool IsLast = (i == Cases.size() - 1);
        BasicBlock *NextBB = IsLast ? DefaultBB : BasicBlock::Create(F->getContext(), "", F);
        IRBuilder<> Builder(CurrentBB);
        Value *Cmp = Builder.CreateICmpEQ(Cond, Cases[i].first);
        Builder.CreateCondBr(Cmp, Cases[i].second, NextBB);
        CurrentBB = NextBB;
      }
      return true;
    }

    bool runOnFunction(Function &F) {
      std::vector<SwitchInst *> Switches;
      for (BasicBlock &BB : F)
          if (auto *SI = dyn_cast<SwitchInst>(BB.getTerminator()))
            Switches.push_back(SI);

      bool Changed = false;
      for (SwitchInst *SI : Switches)
        Changed |= processSwitchInst(SI);
      return Changed;
    }
  };
}

char LowerSwitchPass::ID = 0;
static RegisterPass<LowerSwitchPass> X("lower-switch", "Lower Switch to Branch Pass", false, false);