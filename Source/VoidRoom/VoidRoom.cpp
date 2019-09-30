// Fill out your copyright notice in the Description page of Project Settings.

#include "VoidRoom.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Logging/LogMacros.h"
#include "Misc/Paths.h"

//#define LOCTEXT_NAMESPACE "FILikeTrainsModule"

void FVoidRoomModule::StartupModule()
{
    FString ShaderDirectory = FPaths::Combine(FPaths::ProjectDir(), TEXT("Shaders"));
    AddShaderSourceDirectoryMapping("/Project", ShaderDirectory);
}

void FVoidRoomModule::ShutdownModule()
{
    ResetAllShaderSourceDirectoryMappings();
}

IMPLEMENT_PRIMARY_GAME_MODULE(FVoidRoomModule, VoidRoom, "VoidRoom" );
