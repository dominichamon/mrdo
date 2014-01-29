#include "engine.h"

#include <iostream>

#include <llvm/Analysis/Passes.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/PassManager.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Transforms/Scalar.h>

#include "ast.h"
#include "lexer.h"
#include "parser.h"

namespace dolib {
namespace engine {
namespace {
llvm::ExecutionEngine* execution_engine = nullptr;
llvm::FunctionPassManager* fpm = nullptr;
}
llvm::Module* module = nullptr;

void Initialize() {
  llvm::InitializeNativeTarget();

  module = new llvm::Module("do jit", llvm::getGlobalContext());
  if (!module) {
    std::cerr << "Failed to create module\n";
    exit(1);
  }

  std::string err_str;
  execution_engine = llvm::EngineBuilder(module).setErrorStr(&err_str).create();
  if (!execution_engine) {
    std::cerr << "Failed to create execution engine: " << err_str << "\n";
    exit(1);
  }

#ifndef DEBUG
  fpm = new llvm::FunctionPassManager(module);
  fpm->add(new llvm::DataLayout(*(execution_engine->getDataLayout())));
  fpm->add(llvm::createBasicAliasAnalysisPass());
  fpm->add(llvm::createCFGSimplificationPass());
  fpm->add(llvm::createGVNPass());
  fpm->add(llvm::createInstructionCombiningPass());
  fpm->add(llvm::createPromoteMemoryToRegisterPass());
  fpm->add(llvm::createReassociatePass());
  fpm->doInitialization();
#endif

  lexer::Initialize();
}

void Run() {
  if (ast::Program* p = parser::Program()) {
    if (llvm::Function* lf = p->Codegen()) {

      if (fpm) {
        lf->dump();
        std::cerr << "Optimizing...\n";
        fpm->run(*lf);
        lf->dump();
      }

      void* fptr = engine::execution_engine->getPointerToFunction(lf);
      double (*fp)() = (double (*)())(intptr_t)fptr;

      std::cerr << "Evaluates to: " << fp() << "\n";
    } else {
      std::cerr << "Failed to codegen\n";
    }
  } else {
    std::cerr << "Failed to parse\n";
  }
  module->dump();
}
}  // end namespace engine
}  // end namespace dolib