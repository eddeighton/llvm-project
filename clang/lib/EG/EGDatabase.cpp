

#include "clang/EG/EGDatabase.h"

#include "clang_plugin/clang_plugin.hpp"

#include "clang/AST/Type.h"
#include "clang/Lex/Preprocessor.h"

#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/raw_ostream.h"

#define PLUGIN_ERROR(msg)                                                      \
  llvm::errs() << msg << "\n";                                                 \
  std::abort();

namespace clang {

namespace clang_eg {

static llvm::sys::DynamicLibrary g_eg_plugin;

::mega::EG_PLUGIN_INTERFACE *g_eg_plugin_interface = nullptr;

void eg_load_plugin(const char *strPluginPath) {
  bool bEmptyString = false;
  {
    if (nullptr == strPluginPath)
      bEmptyString = true;
    else if (*strPluginPath == '\0')
      bEmptyString = true;
  }

  if (!bEmptyString) {
    // llvm::errs() << "Got plugin path of: " << strPluginPath << "\n";

    if (nullptr == g_eg_plugin_interface) {
      g_eg_plugin =
          llvm::sys::DynamicLibrary::getPermanentLibrary(strPluginPath);

      if (!g_eg_plugin.isValid()) {
        // error
        PLUGIN_ERROR("Failed to load eg clang plugin dll:" << strPluginPath);
      }

      EG_PLUGIN_INTERFACE_GETTER pGetter =
          reinterpret_cast<EG_PLUGIN_INTERFACE_GETTER>(
              g_eg_plugin.getAddressOfSymbol("GET_EG_PLUGIN_INTERFACE"));
      if (!pGetter) {
        // error
        PLUGIN_ERROR("Failed to get GET_EG_PLUGIN_INTERFACE symbol from eg "
                     "clang plugin dll: "
                     << strPluginPath);
      }

      g_eg_plugin_interface =
          static_cast<::mega::EG_PLUGIN_INTERFACE *>(pGetter());
      if (!g_eg_plugin_interface) {
        // error
        PLUGIN_ERROR("Failed to acquire eg clang plugin interface from dll:"
                     << strPluginPath);
      }
    }
  }
}

void eg_initialise(ASTContext *pASTContext, Sema *pSema) {
  if (g_eg_plugin_interface) {
    g_eg_plugin_interface->initialise(pASTContext, pSema);
  } else {
    // this is not an error - normal usage of the compiler is allowed without
    // needing to specify the clang_plugin dll
  }
}

void eg_setMode(const char *strMode, const char *strSrcDir,
                const char *strBuildDir, const char *strSourceFile) {
  if (g_eg_plugin_interface) {
    g_eg_plugin_interface->setMode(strMode, strSrcDir, strBuildDir,
                                      strSourceFile);
  } else {
    // this is not an error - normal usage of the compiler is allowed without
    // needing to specify the clang_plugin dll
  }
}

void eg_runFinalAnalysis() {
  if (g_eg_plugin_interface) {
    g_eg_plugin_interface->runFinalAnalysis();
  } else {
    PLUGIN_ERROR("eg_runFinalAnalysis called when no eg plugin interface");
  }
}

const char *eg_getTypePathString() { return mega::EG_TYPE_PATH; }
const char *eg_getInvocationString() { return mega::EG_INVOCATION_TYPE; }
const char *eg_getVariantString() { return mega::EG_VARIANT_TYPE; }
const char *eg_getInvokeString() {
  return mega::EG_INVOKE_MEMBER_FUNCTION_NAME;
}
const char *eg_getResultTypeTrait() { return mega::EG_RESULT_TRAIT_TYPE; }

bool eg_isEGEnabled() {
  if (g_eg_plugin_interface) {
    return g_eg_plugin_interface->isEGEnabled();
  } else {
    return false;
  }
}

bool eg_isEGType(const QualType &type) {
  if (g_eg_plugin_interface) {
    return g_eg_plugin_interface->isEGType(type);
  } else {
    PLUGIN_ERROR("eg_isEGType called when no eg plugin interface");
  }
}

bool eg_isPossibleEGType(const QualType &type) {
  if (g_eg_plugin_interface) {
    return g_eg_plugin_interface->isPossibleEGType(type);
  } else {
    PLUGIN_ERROR("eg_isPossibleEGType called when no eg plugin interface");
  }
}

bool eg_isPossibleEGTypeIdentifier(const Token &token) {
  if (g_eg_plugin_interface) {
    return g_eg_plugin_interface->isPossibleEGTypeIdentifier(token);
  } else {
    PLUGIN_ERROR(
        "eg_isPossibleEGTypeIdentifier called when no eg plugin interface");
  }
}

int eg_isPossibleEGTypeIdentifierDecl(const Token &token,
                                      bool bIsTypePathParsing) {
  if (g_eg_plugin_interface) {
    return g_eg_plugin_interface->isPossibleEGTypeIdentifierDecl(
        token, bIsTypePathParsing);
  } else {
    PLUGIN_ERROR(
        "eg_isPossibleEGTypeIdentifierDecl called when no eg plugin interface");
  }
}

bool eg_getInvocationOperationType(const SourceLocation &loc,
                                   const QualType &typePathType,
                                   bool bHasArguments,
                                   QualType &operationType) {
  if (g_eg_plugin_interface) {
    return g_eg_plugin_interface->getInvocationOperationType(
        loc, typePathType, bHasArguments, operationType);
  } else {
    PLUGIN_ERROR(
        "eg_getInvocationOperationType called when no eg plugin interface");
  }
}

bool eg_getInvocationResultType(const SourceLocation &loc,
                                const QualType &baseType,
                                QualType &resultType) {
  if (g_eg_plugin_interface) {
    return g_eg_plugin_interface->getInvocationResultType(loc, baseType,
                                                          resultType);
  } else {
    return false;
    // PLUGIN_ERROR( "eg_getInvocationResultType called when no eg plugin
    // interface" );
  }
}

} // namespace clang_eg

class MegaPragmaHandler : public clang::PragmaHandler
{
public:
    MegaPragmaHandler()
        : PragmaHandler( "mega" )
    {
    }
    void HandlePragma( Preprocessor& PP, PragmaIntroducer Introducer, Token& PragmaTok )
    {
        if( clang_eg::g_eg_plugin_interface ) {
            clang_eg::g_eg_plugin_interface->onMegaPragma();
        }
    }
};

static PragmaHandlerRegistry::Add< MegaPragmaHandler > Y( "mega", "mega pragma description" );

} // namespace clang
