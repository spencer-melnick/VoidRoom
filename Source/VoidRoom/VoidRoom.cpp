// Fill out your copyright notice in the Description page of Project Settings.

#include "VoidRoom.h"
#include "Modules/ModuleManager.h"
#include "Logging/LogMacros.h"
#include "Misc/Paths.h"

DEFINE_LOG_CATEGORY(LogVD);

void FVoidRoomModule::StartupModule()
{
    FString ShaderDirectory = FPaths::Combine(FPaths::ProjectDir(), TEXT("Shaders"));
    AddShaderSourceDirectoryMapping("/Project", ShaderDirectory);
}

void FVoidRoomModule::ShutdownModule()
{
    ResetAllShaderSourceDirectoryMappings();
}

bool FVoidRoomModule::IsGameModule() const
{
    return true;
}

IMPLEMENT_PRIMARY_GAME_MODULE(FVoidRoomModule, VoidRoom, "VoidRoom" );
