// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "CMP400_Prof_ProjCharacter.h"
#include "CMP400_Prof_ProjProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Logging/StructuredLog.h"
#include "Camera/CameraComponent.h"
#include "Engine/HitResult.h" //Might not need this idk
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY(LogWeaponComponent);

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}

void UTP_WeaponComponent::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	//// Try and fire a projectile
	//if (ProjectileClass != nullptr)
	//{
	//	UWorld* const World = GetWorld();
	//	if (World != nullptr)
	//	{
	//		APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
	//		const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
	//		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
	//		const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
	//
	//		//Set Spawn Collision Handling Override
	//		FActorSpawnParameters ActorSpawnParams;
	//		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	//
	//		// Spawn the projectile at the muzzle
	//		World->SpawnActor<ACMP400_Prof_ProjProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	//	}
	//}
	//
	//// Try and play the sound if specified
	//if (FireSound != nullptr)
	//{
	//	UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	//}

	//TODO Convert RPC_Shoot to C++
	//UE_LOG(LogWeaponComponent, Warning, TEXT("FIRE"));

	UCameraComponent* camRef = Character->GetFirstPersonCameraComponent();
	FVector startPos = camRef->GetComponentLocation();
	FVector endPos = startPos + (camRef->GetForwardVector() * weaponRange);

	DrawDebugLine(GetWorld(), startPos, endPos, FColor::Red, false, 5.0f);

	if (owner->HasAuthority()) {
		FHitResult hitResult;
		FCollisionQueryParams collisionParams;
		collisionParams.AddIgnoredActor(Character);

		if (GetWorld()->LineTraceSingleByChannel(hitResult, startPos, endPos, ECollisionChannel::ECC_Visibility, collisionParams)) {
			//UE_LOG(LogWeaponComponent, Warning, TEXT("Hit Actor: %s"), *hitResult.GetActor()->GetName());
			//UClass* hitClass = hitResult.GetActor()->GetClass();
			//FString hitClassName = hitClass->GetName();
			//UE_LOG(LogWeaponComponent, Warning, TEXT("Hit Actor: %s"), *hitClassName);

			if (hitResult.GetActor()->GetClass() == Character->GetClass()) {
				UE_LOG(LogWeaponComponent, Warning, TEXT("Hit Character"));
				hitCharacter = Cast<ACMP400_Prof_ProjCharacter>(hitResult.GetActor());

				if (hitCharacter) {
					UE_LOG(LogWeaponComponent, Warning, TEXT("HIT CAST SUCCESS"));
					hitCharacter->Damage(10);
					Character->Heal();
				}
				else {
					UE_LOG(LogWeaponComponent, Warning, TEXT("HIT CAST FAIL"));
				}
			}
		}
	}
	else {
		FireServer(startPos, endPos);
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void UTP_WeaponComponent::FireServer_Implementation(FVector StartPos, FVector EndPos)
{
	FHitResult hitResult;
	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(Character);

	if (GetWorld()->LineTraceSingleByChannel(hitResult, StartPos, EndPos, ECollisionChannel::ECC_Visibility, collisionParams)) {
		//UE_LOG(LogWeaponComponent, Warning, TEXT("Hit Actor: %s"), *hitResult.GetActor()->GetName());
		//UClass* hitClass = hitResult.GetActor()->GetClass();
		//FString hitClassName = hitClass->GetName();
		//UE_LOG(LogWeaponComponent, Warning, TEXT("Hit Actor: %s"), *hitClassName);

		if (hitResult.GetActor()->GetClass() == Character->GetClass()) {
			UE_LOG(LogWeaponComponent, Warning, TEXT("Hit Character"));
			hitCharacter = Cast<ACMP400_Prof_ProjCharacter>(hitResult.GetActor());

			if (hitCharacter) {
				UE_LOG(LogWeaponComponent, Warning, TEXT("HIT CAST SUCCESS"));
				hitCharacter->Damage(10);
			}
			else {
				UE_LOG(LogWeaponComponent, Warning, TEXT("HIT CAST FAIL"));
			}
		}
	}
}

void UTP_WeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	USceneComponent* attachParent = GetAttachParent();
	FString attachParentString = "No Name";

	owner = GetOwner();
	FString ownerString = "NO OWNER NAME";

	Character = Cast<ACMP400_Prof_ProjCharacter>(owner);

	if (Character) {
		UE_LOG(LogWeaponComponent, Warning, TEXT("CAST SUCCESS"));
	}
	else {
		UE_LOG(LogWeaponComponent, Warning, TEXT("CAST FAIL"));
	}

	/*if (attachParent != nullptr) {
		attachParentString = attachParent->GetName();
		if (IsNetMode(NM_Client) && !IsNetMode(NM_ListenServer)) {
			UE_LOG(LogWeaponComponent, Warning, TEXT("CLIENT - OWNER %s"), *attachParentString);
		}
		if (IsNetMode(NM_ListenServer)) {
			UE_LOG(LogWeaponComponent, Warning, TEXT("LISTEN SERVER - OWNER %s"), *attachParentString);
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("OWNER NULLPTR"));
	}*/

	if (owner != nullptr) {
		ownerString = owner->GetName();
		if (IsNetMode(NM_Client) && !IsNetMode(NM_ListenServer)) {
			//UE_LOG(LogWeaponComponent, Warning, TEXT("CLIENT - OWNER %s"), *ownerString);
		}
		if (IsNetMode(NM_ListenServer)) {
			//UE_LOG(LogWeaponComponent, Warning, TEXT("LISTEN SERVER - OWNER %s"), *ownerString);
		}
	}
	else {
		//UE_LOG(LogWeaponComponent, Warning, TEXT("OWNER NULLPTR"));
	}

}

bool UTP_WeaponComponent::AttachWeapon(ACMP400_Prof_ProjCharacter* TargetCharacter)
{
	Character = TargetCharacter;

	//UE_LOGFMT(LogTemp, Log, "Printing my Name with Value");
	//UE_LOG(LogTemp, Display, TEXT("Hello"));
	//UE_LOG(LogTemp, Warning, TEXT("Hello %s"), *Character->GetActorNameOrLabel());

	// Check that the character is valid, and has no weapon component yet
	if (Character == nullptr || Character->GetInstanceComponents().FindItemByClass<UTP_WeaponComponent>())
	{
		return false;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

	// add the weapon as an instance component to the character
	Character->AddInstanceComponent(this);

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Fire);
		}
	}

	return true;
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}

void UTP_WeaponComponent::roundReset()
{
	//Reset ammo in blueprints
}
