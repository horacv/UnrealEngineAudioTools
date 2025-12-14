#pragma once

#include "Modules/ModuleManager.h"

class FANS_EditorAudioToolsUEModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
