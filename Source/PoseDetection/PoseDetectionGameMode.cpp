// Copyright Epic Games, Inc. All Rights Reserved.

#include "PoseDetectionGameMode.h"
#include "PoseDetectionCharacter.h"
#include "UObject/ConstructorHelpers.h"

APoseDetectionGameMode::APoseDetectionGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
