// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "OceanSim.h"

#include "GlobalShader.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FOceanSimModule"

void FOceanSimModule::StartupModule()
{
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("OceanSim"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/OceanShaders"), PluginShaderDir);
}

void FOceanSimModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOceanSimModule, OceanSim)