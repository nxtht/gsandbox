// Copyright Epic Games, Inc. All Rights Reserved.

#include "GSandboxGameModeBase.h"
#include "SandboxPawn.h"

AGSandboxGameModeBase::AGSandboxGameModeBase()
{
    DefaultPawnClass = ASandboxPawn::StaticClass();
}