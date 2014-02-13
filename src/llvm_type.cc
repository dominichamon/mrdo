#include "llvm_type.h"

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>

#include "builtin.h"

template <> llvm::Type* TypeMap<void>::get() {
  return llvm::Type::getVoidTy(llvm::getGlobalContext());
}

template <> llvm::Type* TypeMap<double>::get() {
  return llvm::Type::getDoubleTy(llvm::getGlobalContext());
}

template <> llvm::Type* TypeMap<builtin::Collection>::get() {
  return llvm::StructType::get(
      llvm::PointerType::getUnqual(TypeMap<double>::get()),
      llvm::Type::getInt64Ty(llvm::getGlobalContext()),
      nullptr);
}

