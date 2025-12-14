#include "ANS_EditorAudioToolsUE.h"

#define LOCTEXT_NAMESPACE "FHV_EditorAudioToolsModule"

void FANS_EditorAudioToolsUEModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FANS_EditorAudioToolsUEModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FANS_EditorAudioToolsUEModule, ANS_EditorAudioToolsUE)