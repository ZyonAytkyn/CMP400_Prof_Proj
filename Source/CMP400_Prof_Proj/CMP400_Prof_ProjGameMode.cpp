// Copyright Epic Games, Inc. All Rights Reserved.

#include "CMP400_Prof_ProjGameMode.h"
#include "CMP400_Prof_ProjCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACMP400_Prof_ProjGameMode::ACMP400_Prof_ProjGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
