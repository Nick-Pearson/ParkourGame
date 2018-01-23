#pragma once


#include "ILowEntryJsonModule.h"


class FLowEntryJsonModule : public ILowEntryJsonModule
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
