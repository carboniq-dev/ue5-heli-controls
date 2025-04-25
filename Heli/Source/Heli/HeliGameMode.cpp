// Copyright Epic Games, Inc. All Rights Reserved.

#include "HeliGameMode.h"
#include "UObject/ConstructorHelpers.h"

AHeliGameMode::AHeliGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Heli/Blueprints/BP_Heli"));

	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
