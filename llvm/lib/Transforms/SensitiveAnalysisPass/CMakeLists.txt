add_llvm_library( LLVMSensitive MODULE
  Sensitive.cpp

  DEPENDS
  intrinsics_gen
  PLUGIN_TOOL
  opt
  )