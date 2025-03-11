// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CMP400_Prof_ProjPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class CMP400_PROF_PROJ_API ACMP400_Prof_ProjPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Replicated)
	float timerModifier = 1.0f;
	float healthModifier = 1.0f;
};
