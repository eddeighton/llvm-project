set(LLVM_LINK_COMPONENTS
  Support
  )

find_path( MEGA_API_DIR_INCLUDE NAMES clang_plugin/clang_plugin.hpp REQUIRED PATHS ${MEGA_API_DIR} )
include_directories( ${MEGA_API_DIR_INCLUDE} )

add_clang_library(clangEG
  EGDatabase.cpp

  LINK_LIBS
  clangAST
  #clangAnalysis
  #clangBasic
  #clangEdit
  #clangFrontend
  #clangLex
  #clangRewrite
  #clangSerialization
  #clangStaticAnalyzerCheckers
  #clangStaticAnalyzerCore
  )
