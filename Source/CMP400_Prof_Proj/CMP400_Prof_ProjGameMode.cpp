// Copyright Epic Games, Inc. All Rights Reserved.

#include "CMP400_Prof_ProjGameMode.h"
#include "CMP400_Prof_ProjCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include <Net/UnrealNetwork.h>

ACMP400_Prof_ProjGameMode::ACMP400_Prof_ProjGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	this->bUseSeamlessTravel = true;
}

void ACMP400_Prof_ProjGameMode::updateScore(int team)
{
	//Do this on the server
	teamScore[team] += 1;
	AGameStateBase* gameState = GetWorld()->GetGameState();
	TArray<APlayerState*> player_array = gameState->PlayerArray;
	for (auto& i : Players) {
		Cast<ACMP400_Prof_ProjCharacter>(i->GetPawn())->teamScore = teamScore;
	}
	UE_LOG(LogTemp, Warning, TEXT("TEAM: %f"), static_cast<float>(team));
	UE_LOG(LogTemp, Warning, TEXT("SERVER SCORE %f"), static_cast<float>(teamScore[0]));
	UE_LOG(LogTemp, Warning, TEXT("CLIENT SCORE %f"), static_cast<float>(teamScore[1]));
}

void ACMP400_Prof_ProjGameMode::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(gameTime, this, &ACMP400_Prof_ProjGameMode::EndMatch, matchLength, false);
}

void ACMP400_Prof_ProjGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (!HasWaited)
	{
		FTimerHandle timerhandler;
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "BindTime", NewPlayer);
		GetWorld()->GetTimerManager().SetTimer(timerhandler, Delegate, 1.0f, false);
		AGameStateBase* gameState = GetWorld()->GetGameState();
		Cast<ACMP400_Prof_ProjCharacter>(NewPlayer->GetPawn())->playerTeam = gameState->PlayerArray.Num() - 1;
		Players.Add(NewPlayer);
	}
}

void ACMP400_Prof_ProjGameMode::BindTime(APlayerController* NewPlayer)
{
	Cast<ACMP400_Prof_ProjCharacter>(NewPlayer->GetPawn())->OnScoreChange.AddUniqueDynamic(this, &ACMP400_Prof_ProjGameMode::updateScore);
}

void ACMP400_Prof_ProjGameMode::EndMatch()
{
	OnRoundEnd.Broadcast();
	UE_LOG(LogTemp, Warning, TEXT("MATCH ENDED"));
}

void ACMP400_Prof_ProjGameMode::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACMP400_Prof_ProjGameMode, teamScore);
	DOREPLIFETIME(ACMP400_Prof_ProjGameMode, roundEnded);
}