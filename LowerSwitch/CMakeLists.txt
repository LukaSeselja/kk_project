add_llvm_library( LLVMLowerSwitch MODULE
  LowerSwitchPass.cpp

  PLUGIN_TOOL
  opt
  )
