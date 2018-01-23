#pragma once


#include "ILowEntryJsonEditorModule.h"


class FLowEntryJsonEditorModule : public ILowEntryJsonEditorModule
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
