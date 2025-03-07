// Copyright Epic Games, Inc. All Rights Reserved.

#include "CMP400_Prof_ProjCharacter.h"
#include "CMP400_Prof_ProjProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include <Net/UnrealNetwork.h>
#include "Misc/OutputDeviceNull.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ACMP400_Prof_ProjCharacter

ACMP400_Prof_ProjCharacter::ACMP400_Prof_ProjCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

}

void ACMP400_Prof_ProjCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APlayerStart::StaticClass(), TEXT("Server"), serverStarts);
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APlayerStart::StaticClass(), TEXT("Client"), clientStarts);
	
	/*int random = FMath::RandRange(0, 2);
	if (playerTeam == ServerTeam) {
		SetActorLocation(serverStarts[random]->GetActorLocation());
	}
	if (playerTeam == ClientTeam) {
		SetActorLocation(clientStarts[random]->GetActorLocation());
	}*/
}

//////////////////////////////////////////////////////////////////////////// Input

void ACMP400_Prof_ProjCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACMP400_Prof_ProjCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACMP400_Prof_ProjCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

float ACMP400_Prof_ProjCharacter::GetHealth()
{
	return Health;
}

void ACMP400_Prof_ProjCharacter::OnDeath()
{
	//GET ALL PLAYER START
	//SELECT RANDOM FROM ARRAY
	//TELEPORT PLAYER TO SELECTED LOCATION

	int random = FMath::RandRange(0, 2);
	if (playerTeam == ServerTeam) {
		SetActorLocation(serverStarts[random]->GetActorLocation());
	}
	if (playerTeam == ClientTeam) {
		SetActorLocation(clientStarts[random]->GetActorLocation());
	}

	//SetActorLocation(GetActorLocation() + 100);
	Health = 100;
	//UE_LOG(LogTemplateCharacter, Warning, TEXT("YOU ARE DEAD"));
}

void ACMP400_Prof_ProjCharacter::Damage(float damage)
{
	Health -= damage;
	//UE_LOG(LogTemplateCharacter, Warning, TEXT("Hit Actor: %f"), Health);
	if (Health <= 0)
	{
		/* Kill feed stuff, low priority
		FOutputDeviceNull ar;
		this->CallFunctionByNameWithArguments(TEXT("Dead_BP"), ar, NULL, true);*/

		if (playerTeam == ServerTeam) {
			OnScoreChange.Broadcast(1);
		}
		if (playerTeam == ClientTeam) {
			OnScoreChange.Broadcast(0);
		}

		OnDeath();
	}
}


void ACMP400_Prof_ProjCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ACMP400_Prof_ProjCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ACMP400_Prof_ProjCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACMP400_Prof_ProjCharacter, Health);
	DOREPLIFETIME(ACMP400_Prof_ProjCharacter, teamScore);
}