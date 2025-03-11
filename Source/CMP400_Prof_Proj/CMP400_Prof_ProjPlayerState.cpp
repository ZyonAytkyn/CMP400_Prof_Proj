// Fill out your copyright notice in the Description page of Project Settings.


#include "CMP400_Prof_ProjPlayerState.h"
#include <Net/UnrealNetwork.h>

ACMP400_Prof_ProjPlayerState::ACMP400_Prof_ProjPlayerState()
{
}

void ACMP400_Prof_ProjPlayerState::CopyProperties(class APlayerState* PlayerState)
{
    Super::CopyProperties(PlayerState);

    if (IsValid(PlayerState))
    {
        ACMP400_Prof_ProjPlayerState* TestPlayerState = Cast<ACMP400_Prof_ProjPlayerState>(PlayerState);
        if (IsValid(TestPlayerState))
        {
            TestPlayerState->timerModifier = timerModifier;
            TestPlayerState->healthModifier = healthModifier;
        }
    }
}

void ACMP400_Prof_ProjPlayerState::OverrideWith(class APlayerState* PlayerState)
{
    Super::OverrideWith(PlayerState);

    if (IsValid(PlayerState))
    {
        ACMP400_Prof_ProjPlayerState* TestPlayerState = Cast<ACMP400_Prof_ProjPlayerState>(PlayerState);
        if (IsValid(TestPlayerState))
        {
            timerModifier = TestPlayerState->timerModifier;
            healthModifier = TestPlayerState->healthModifier;
        }
    }
}

void ACMP400_Prof_ProjPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACMP400_Prof_ProjPlayerState, timerModifier);
	DOREPLIFETIME(ACMP400_Prof_ProjPlayerState, healthModifier);
}