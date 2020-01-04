// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogVD, Log, All);

class FVoidRoomModule: public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    virtual bool IsGameModule() const override;
};

namespace VDGame
{
    const char StencilOutlineFlag = 0x2;

    const ECollisionChannel FocusOpaqueChannel = ECollisionChannel::ECC_GameTraceChannel1;
}
