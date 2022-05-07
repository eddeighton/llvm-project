set(LLVM_LINK_COMPONENTS
  FrontendOpenMP
  Support
  )

add_clang_library(clangChangeNamespace
  ChangeNamespace.cpp

  DEPENDS
  omp_gen
)

clang_target_link_libraries(clangChangeNamespace
  PRIVATE
  clangAST
  clangASTMatchers
  clangBasic
  clangFormat
  clangFrontend
  clangLex
  clangSerialization
  clangTooling
  clangToolingCore
  clangEG
  )

add_subdirectory(tool)
