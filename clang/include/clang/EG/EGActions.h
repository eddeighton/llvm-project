//===-- EGActions.h - EG Frontend Action for EG compilation ---------------===//


#ifndef LLVM_CLANG_EG_ACTIONS_H
#define LLVM_CLANG_EG_ACTIONS_H

#include "clang/EG/EGDatabase.h"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/MultiplexConsumer.h"
#include "clang/Frontend/FrontendAction.h"

#include <memory>
#include <vector>

namespace clang {

namespace clang_eg {
     
class eg_action : public WrapperFrontendAction 
{
protected:
    bool BeginInvocation(CompilerInstance &CI) override
    {
        eg_load_plugin( CI.getFrontendOpts().EGPluginDllPath.c_str() );
        
        if( !CI.getFrontendOpts().EGDatabasePath.empty() )
        {
            if( CI.getFrontendOpts().EGTranslationUnitDatabasePath.empty() )
            {
                eg_initialiseMode_Interface( CI.getFrontendOpts().EGDatabasePath.c_str() );
            }
            else
            {
                eg_initialiseMode_Operations( 
                    CI.getFrontendOpts().EGDatabasePath.c_str(),
                    CI.getFrontendOpts().EGTranslationUnitDatabasePath.c_str(),
                    CI.getFrontendOpts().EGTranslationUnitID );
            }
        }
        else
        {
            eg_initialiseMode_Implementation();
        }
        
        return true;
    }

    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef InFile) override
    {
        std::vector< std::unique_ptr< ASTConsumer> > Consumers;
      
        Consumers.push_back( WrapperFrontendAction::CreateASTConsumer( CI, InFile ) );
        
        return std::make_unique< MultiplexConsumer >( std::move( Consumers ) );
    }
    
    //This is only called if the compilation was successful
    virtual void EndSourceFileAction()
    {
        WrapperFrontendAction::EndSourceFileAction();
        
        eg_runFinalAnalysis();
    }
    
public:
    eg_action( CompilerInstance &CI, std::unique_ptr< FrontendAction > WrappedAction )
    :   WrapperFrontendAction( std::move( WrappedAction ) )
    {
    }
    
};

} // end namespace clang_eg

}  // end namespace clang

#endif