// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Logging/LogMacros.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "CMP400_Prof_ProjCharacter.h"
#include "CMP400_Prof_ProjGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRoundEnd);

UCLASS(minimalapi) 
class ACMP400_Prof_ProjGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACMP400_Prof_ProjGameMode();
	
	UFUNCTION()
	void updateScore(int team);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Replicated)
	TArray<int> teamScore = TArray<int>{ 0,0 };

	bool HasWaited = false;

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnRoundEnd OnRoundEnd;

protected:
	UFUNCTION()
	virtual void BeginPlay();

	UFUNCTION()
	virtual void PostLogin(APlayerController* NewPlayer) override;

	UFUNCTION()
	void BindTime(APlayerController* NewPlayer);

	UFUNCTION()
	void EndMatch();

	TArray<APlayerController*> Players;

	FTimerHandle gameTime;

	//float matchLength = 300.0f;
	float matchLength = 43200.0f; //12 HOURS!!!!! FOR GRAD SHOW!!!!!

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	bool roundEnded = false;
};



