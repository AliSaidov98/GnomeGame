// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoopGnomeCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "WeaponBase.h"
#include "Interaction/InteractSphereComponent.h"
#include "InventoryComponent.h"
#include "GameStates/CoopGnomePlayerState.h"
#include "CoopGnomeGameMode.h"
#include "Net/UnrealNetwork.h"
#include "OnlineSubsystem.h"
#include "Components/CombatComponent.h"
#include "Weapon/Weapon.h"


#include "Interfaces/OnlineSessionInterface.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ACoopGnomeCharacter

ACoopGnomeCharacter::ACoopGnomeCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));
	Combat->SetIsReplicated(true);


	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();

	if (OnlineSubsystem)
	{
		OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();

		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15,
				FColor::Green,
				FString::Printf(TEXT("Found subsystem %s"), *OnlineSubsystem->GetSubsystemName().ToString()));
		}
	}

}

void ACoopGnomeCharacter::ServerLeaveGame_Implementation()
{
	CoopGnomeGameMode = CoopGnomeGameMode == nullptr ? GetWorld()->GetAuthGameMode<ACoopGnomeGameMode>() : CoopGnomeGameMode;
	CoopGnomePlayerState = CoopGnomePlayerState == nullptr ? GetPlayerState<ACoopGnomePlayerState>() : CoopGnomePlayerState;
	if (CoopGnomeGameMode && CoopGnomePlayerState)
	{
		CoopGnomeGameMode->PlayerLeftGame(CoopGnomePlayerState);
	}
}

void ACoopGnomeCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACoopGnomeCharacter, IsEquippedWeapon);
	DOREPLIFETIME(ACoopGnomeCharacter, ControlRotation);
	DOREPLIFETIME(ACoopGnomeCharacter, ActorRotation);
	DOREPLIFETIME_CONDITION(ACoopGnomeCharacter, OverlappingWeapon, COND_OwnerOnly);
}


bool ACoopGnomeCharacter::IsWeaponEquipped()
{
	return true;
}

bool ACoopGnomeCharacter::IsAiming()
{
	return false;
}

AWeapon* ACoopGnomeCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}

void ACoopGnomeCharacter::ServerInteractPressed_Implementation()
{
	if(Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
}

void ACoopGnomeCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ACoopGnomeCharacter::PlayReloadMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;

		switch (Combat->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_RocketLauncher:
			SectionName = FName("RocketLauncher");
			break;
		case EWeaponType::EWT_Pistol:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_SubmachineGun:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_Shotgun:
			SectionName = FName("Shotgun");
			break;
		case EWeaponType::EWT_SniperRifle:
			SectionName = FName("SniperRifle");
			break;
		case EWeaponType::EWT_GrenadeLauncher:
			SectionName = FName("GrenadeLauncher");
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ACoopGnomeCharacter::PlayDeathMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
	}
}

void ACoopGnomeCharacter::PlaySwapMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && SwapMontage)
	{
		AnimInstance->Montage_Play(SwapMontage);
	}
}


void ACoopGnomeCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	SetupInventory();
}

void ACoopGnomeCharacter::OnPlayerStateInitialized()
{
	CoopGnomePlayerState->AddToScore(0.f);
	CoopGnomePlayerState->AddToDefeats(0);
}

void ACoopGnomeCharacter::SetupInventory()
{
	InventoryComponent = FindComponentByClass<UInventoryComponent>();
	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACoopGnomeCharacter::SetupInventory(): InventoryComponent is not valid!"));		
		return;
	}

	InventoryComponent->OnWeaponEquipped.AddDynamic(this, &ACoopGnomeCharacter::EquipWeapon);
	InventoryComponent->OnWeaponUnequipped.AddDynamic(this, &ACoopGnomeCharacter::UnequipWeapon);
}

void ACoopGnomeCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Combat)
	{
		Combat->Character = this;
	}
}

void ACoopGnomeCharacter::EquipWeapon(FString WeaponName)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
		EquippedWeapon = nullptr;
	}
	
	FItemStruct WeaponItem;
	if (!InventoryComponent->GetItemByName(WeaponName, WeaponItem))
	{
		UE_LOG(LogTemp, Warning, TEXT("ACoopGnomeCharacter::EquipWeapon(): WeaponItem is not found!"));
		return;
	}

	if (!WeaponItem.WeaponClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACoopGnomeCharacter::EquipWeapon(): WeaponClass is not valid!"));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	EquippedWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponItem.WeaponClass, GetActorTransform(), SpawnParams);
	
	if (!EquippedWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACoopGnomeCharacter::EquipWeapon(): EquippedWeapon is not valid!"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT(" ACoopGnomeCharacter::EquipWeapon():  EquippedWeapon"));
	EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponAttachSocketName);
	EquippedWeapon->OwnerCharacter = this;
	
	IsEquippedWeapon = true;
}

void ACoopGnomeCharacter::UnequipWeapon(FString WeaponName)
{
	if (!EquippedWeapon)
		return;

	FItemStruct WeaponItem;
	if (!InventoryComponent->GetItemByName(WeaponName, WeaponItem))
	{
		UE_LOG(LogTemp, Warning, TEXT("ACoopGnomeCharacter::UnequipWeapon(): WeaponItem is not found!"));
		return;
	}
	if (!WeaponItem.WeaponClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACoopGnomeCharacter::EquipWeapon(): WeaponClass is not valid!"));
		return;
	}

	if (EquippedWeapon.GetClass() == WeaponItem.WeaponClass)
	{
		EquippedWeapon->Destroy();
		EquippedWeapon = nullptr;
		IsEquippedWeapon = false;
	}
}

void ACoopGnomeCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if(OverlappingWeapon)
		OverlappingWeapon->ShowPickupWidget(false);
	
	OverlappingWeapon = Weapon;

	if(IsLocallyControlled())
	{
		if(OverlappingWeapon)
			OverlappingWeapon->ShowPickupWidget(true);
	}
}

void ACoopGnomeCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if(OverlappingWeapon)
		OverlappingWeapon->ShowPickupWidget(true);
	if(LastWeapon)
		LastWeapon->ShowPickupWidget(false);
}


//////////////////////////////////////////////////////////////////////////
// Input

void ACoopGnomeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACoopGnomeCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACoopGnomeCharacter::Look);
		
		// Attack
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ACoopGnomeCharacter::Attack);
		
		// Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ACoopGnomeCharacter::Interact);

		
		// EquipNextWeapon 
		EnhancedInputComponent->BindAction(EquipNextWeaponAction, ETriggerEvent::Triggered, this, &ACoopGnomeCharacter::EquipNextWeapon);

	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ACoopGnomeCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ACoopGnomeCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);

		ControlRotation = GetControlRotation();
		ActorRotation = GetActorRotation();
	}
}

void ACoopGnomeCharacter::Attack(const FInputActionValue& Value)
{
	if(!EquippedWeapon)
		return;
	
	EquippedWeapon->Attack();
}

void ACoopGnomeCharacter::Interact(const FInputActionValue& Value)
{

	if(Combat)
	{
		if(HasAuthority())
			Combat->EquipWeapon(OverlappingWeapon);
		else
			ServerInteractPressed();
	}
	
	auto InteractComponent = FindComponentByClass<UInteractSphereComponent>();
	
	if (!InteractComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACoopGnomeCharacter::Interact(): No interact component!"));
		return;
	}
	
	bool bInteracted = InteractComponent->Interact();
	
	if (bInteracted)
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("INTERACT SUCCESS")));

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("INTERACT  %hhd "), bInteracted));

}

void ACoopGnomeCharacter::EquipNextWeapon(const FInputActionValue& Value)
{
	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACoopGnomeCharacter::EquipNextWeapon(): InventoryComponent is not valid!"));
		return;
	}

	for (const auto& Item : InventoryComponent->CurrentItems)
	{
		if (!Item.WeaponClass)
			continue;

		if (!EquippedWeapon || Item.WeaponClass != EquippedWeapon.GetClass())
		{
			EquipWeapon(Item.Name);
			return;
		}
	}
}
