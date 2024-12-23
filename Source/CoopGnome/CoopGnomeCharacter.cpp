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
#include "GameModes/CoopGnomeGameMode.h"
#include "HealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "OnlineSubsystem.h"
#include "Components/CombatComponent.h"
#include "Engine/DamageEvents.h"
#include "Weapon/Weapon.h"


#include "Interfaces/OnlineSessionInterface.h"
#include "Player/CoopGnomePlayerController.h"
#include "Types/CollisionState.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ACoopGnomeCharacter

ACoopGnomeCharacter::ACoopGnomeCharacter()
{
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
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

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	NetUpdateFrequency = 100;
	MinNetUpdateFrequency = 33;

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));
	
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
	DOREPLIFETIME(ACoopGnomeCharacter, TurningInPlace);
	DOREPLIFETIME(ACoopGnomeCharacter, AO_YawRep);
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

void ACoopGnomeCharacter::ServerEquipWeaponPressed_Implementation()
{
	if(Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
}

void ACoopGnomeCharacter::ServerInteractPressed_Implementation()
{
	
}

void ACoopGnomeCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bAiming ? FName("RifleAim") : FName("Rifle");
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
	/*UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
	}*/
}

void ACoopGnomeCharacter::PlaySwapMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && SwapMontage)
	{
		AnimInstance->Montage_Play(SwapMontage);
	}
}

void ACoopGnomeCharacter::PlayHitReactMontage()
{
	if (Combat == nullptr /*|| Combat->EquippedWeapon == nullptr*/) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		//FName SectionName;
		//SectionName = FName("RifleAim") : FName("Rifle");
		//AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ACoopGnomeCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);
	}
}

void ACoopGnomeCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
	auto HealthComponent = FindComponentByClass<UHealthComponent>();
	if (!HealthComponent)
		return;

	HealthComponent->OnHealthChange.AddDynamic(this, &ACoopGnomeCharacter::UpdateHealth);

	HealthComponent->OnDeath.AddDynamic(this, &ACoopGnomeCharacter::Elim_Server);
    
	if(HasAuthority())
		OnTakeAnyDamage.AddDynamic(this, &ACoopGnomeCharacter::TakeAnyDamage);
	
	SetupInventory();
}

void ACoopGnomeCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	PollInit();
}

void ACoopGnomeCharacter::OnPlayerStateInitialized()
{
	CoopGnomePlayerState->AddToScore(0.f);
	CoopGnomePlayerState->AddToDefeats(0);
}

void ACoopGnomeCharacter::HideCameraIfCharacterClose()
{
	if(!IsLocallyControlled()) return;

	if((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if(Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if(Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

void ACoopGnomeCharacter::SetupInventory()
{
	InventoryComponent = FindComponentByClass<UInventoryComponent>();
	if (!InventoryComponent)
	{
		//UE_LOG(LogTemp, Warning, TEXT("ACoopGnomeCharacter::SetupInventory(): InventoryComponent is not valid!"));		
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

void ACoopGnomeCharacter::Elim_Server()
{
	if(Combat && Combat->EquippedWeapon)
	{
		Combat->EquippedWeapon->Dropped();
	}
	
	CoopGnomeGameMode = CoopGnomeGameMode == nullptr ? GetWorld()->GetAuthGameMode<ACoopGnomeGameMode>() : CoopGnomeGameMode;

	if(!CoopGnomeGameMode) return;
	
	auto HealthComponent = FindComponentByClass<UHealthComponent>();
	if (!HealthComponent)
		return;

	ACoopGnomePlayerController* CharacterPlayerController = Cast<ACoopGnomePlayerController>(Controller);
	ACoopGnomePlayerController* AttackerController = Cast<ACoopGnomePlayerController>(HealthComponent->Instigator);

	if(CharacterPlayerController)
		CoopGnomeGameMode->PlayerEliminated(this, CharacterPlayerController, AttackerController);
	else
		CoopGnomeGameMode->AIEliminated(this, AttackerController);
		
	ElimMulticast(bLeftGame);
}

void ACoopGnomeCharacter::ElimMulticast_Implementation(bool bPlayerLeftGame)
{

	bLeftGame = bPlayerLeftGame;
	
	Dead = true;
	
	//PlayElimMontage();

	SetupDissolve();

	//Disable character movement

	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();

	bDisableGameplay = true;
	
	if(Combat)
		Combat->FireButtonPressed(false);
	
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	SetActorEnableCollision(true);
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;
	
	//Disable collision
	
	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	GetWorldTimerManager().SetTimer(
		ElimTimer,
		this,
		&ACoopGnomeCharacter::ElimTimerFinished,
		ElimDelay
	);
}

void ACoopGnomeCharacter::ElimTimerFinished()
{
	CoopGnomeGameMode = CoopGnomeGameMode == nullptr ? GetWorld()->GetAuthGameMode<ACoopGnomeGameMode>() : CoopGnomeGameMode;

	if(CoopGnomeGameMode && !bLeftGame)
		CoopGnomeGameMode->RequestRespawn(this, Controller);
	if(bLeftGame && IsLocallyControlled())
		OnLeftGame.Broadcast();
}


void ACoopGnomeCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	/*CoopGnomePlayerState = CoopGnomePlayerState == nullptr ? Cast<ACoopGnomePlayerState>(GetPlayerState()) : CoopGnomePlayerState;

	if(CoopGnomePlayerState)
	{
		if(CoopGnomePlayerState.matchState)
		if(CoopGnomePlayerState)
			CoopGnomePlayerState->GetAnnouncementMessage();
		
	}*/
	
	
	NewPossess();
}

void ACoopGnomeCharacter::NewPossess_Implementation()
{
	if(!IsLocallyControlled()) return;
	
	ACoopGnomePlayerController* PlayerController = Cast<ACoopGnomePlayerController>(Controller);

	if(!PlayerController ) return;
	if(!PlayerController->GetHUD()) return;
	
	Cast<AGameHUD>(PlayerController->GetHUD())->ShowMainInterface();

}

void ACoopGnomeCharacter::SetupDissolve()
{
	if(DissolveMaterialInstance.IsValidIndex(0))
	{
		for (int i = 0; i < DissolveMaterialInstance.Num(); ++i)
		{
			DynamicDissolveMaterialInstance.SetNum(DissolveMaterialInstance.Num());
			
			DynamicDissolveMaterialInstance[i] = UMaterialInstanceDynamic::Create(DissolveMaterialInstance[i], this);
			GetMesh()->SetMaterial(i, DynamicDissolveMaterialInstance[i]);
			DynamicDissolveMaterialInstance[i]->SetScalarParameterValue(TEXT("Alpha"), 0.55f);
			DynamicDissolveMaterialInstance[i]->SetScalarParameterValue(TEXT("EdgePower"), 1500.f);
		}
	}

	StartDissolve();
}

void ACoopGnomeCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	for (int i = 0; i < DynamicDissolveMaterialInstance.Num(); ++i)
	{
		if(DynamicDissolveMaterialInstance[i])
		{
			DynamicDissolveMaterialInstance[i]->SetScalarParameterValue(TEXT("Alpha"), DissolveValue);
		}
	}
}

void ACoopGnomeCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ACoopGnomeCharacter::UpdateDissolveMaterial);

	if(DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}

void ACoopGnomeCharacter::EquipWeapon(FString WeaponName)
{
	/*
	if (OverlappingWeapon)
	{
		OverlappingWeapon->Destroy();
		OverlappingWeapon = nullptr;
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
	OverlappingWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponItem.WeaponClass, GetActorTransform(), SpawnParams);
	
	if(Combat && OverlappingWeapon)
	{
		if(HasAuthority())
			Combat->EquipWeapon(OverlappingWeapon);
		else
			ServerEquipWeaponPressed();
	}*/
	
	/*if (!EquippedWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACoopGnomeCharacter::EquipWeapon(): EquippedWeapon is not valid!"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT(" ACoopGnomeCharacter::EquipWeapon():  EquippedWeapon"));
	EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponAttachSocketName);
	EquippedWeapon->OwnerCharacter = this;
	
	IsEquippedWeapon = true;*/
}

void ACoopGnomeCharacter::UnequipWeapon(FString WeaponName)
{
	if (!OverlappingWeapon)
		return;

	FItemStruct WeaponItem;
	if (!InventoryComponent->GetItemByName(WeaponName, WeaponItem))
	{
		//UE_LOG(LogTemp, Warning, TEXT("ACoopGnomeCharacter::UnequipWeapon(): WeaponItem is not found!"));
		return;
	}
	if (!WeaponItem.WeaponClass)
	{
		//UE_LOG(LogTemp, Warning, TEXT("ACoopGnomeCharacter::EquipWeapon(): WeaponClass is not valid!"));
		return;
	}

	if (OverlappingWeapon.GetClass() == WeaponItem.WeaponClass)
	{
		OverlappingWeapon->Destroy();
		OverlappingWeapon = nullptr;
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

void ACoopGnomeCharacter::SetTurningInPlace(ETurningInPlace TurningType)
{
	TurningInPlace = TurningType;
}

FVector ACoopGnomeCharacter::GetHitTarget() const
{
	if(Combat == nullptr) return FVector();

	return Combat->HitTarget;
}

void ACoopGnomeCharacter::TurnInPlace(float DeltaTime, float AO_Yaw)
{
	AO_YawRep = AO_Yaw;

	if(HasAuthority())
	{
		ControlRotation = GetControlRotation();
		ActorRotation = GetActorRotation();
		
		if(AO_YawRep > 45.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
	
		else if(AO_YawRep < -85.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
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
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ACoopGnomeCharacter::Attack);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &ACoopGnomeCharacter::AttackReleased);
		
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

void ACoopGnomeCharacter::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	PlayHitReactMontage();
}

void ACoopGnomeCharacter::UpdateHealth(float CurrentHealth, float MaxHealth)
{
	//PlayHitReactMontage();
}

void ACoopGnomeCharacter::PollInit()
{
	if(CoopGnomePlayerState == nullptr)
	{
		CoopGnomePlayerState = GetPlayerState<ACoopGnomePlayerState>();

		if(CoopGnomePlayerState)
		{
			CoopGnomePlayerState->AddToScore(0);
		}
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

		
		HideCameraIfCharacterClose();
	}
}

void ACoopGnomeCharacter::Attack(const FInputActionValue& Value)
{
	if(bDisableGameplay) return;
	
	if(Combat)
		Combat->FireButtonPressed(true);
	
	/*if(!EquippedWeapon)
		return;
	
	EquippedWeapon->Attack();*/
}

void ACoopGnomeCharacter::AttackReleased(const FInputActionValue& Value)
{
	if(bDisableGameplay) return;
	
	if(Combat)
		Combat->FireButtonPressed(false);
	/*if(!EquippedWeapon)
		return;
	
	EquippedWeapon->Attack();*/
}

void ACoopGnomeCharacter::EquipCombatWeapon()
{
	if(Combat)
	{
		if(HasAuthority())
			Combat->EquipWeapon(OverlappingWeapon);
		else
			ServerEquipWeaponPressed();
	}
}

void ACoopGnomeCharacter::Interact(const FInputActionValue& Value)
{
	EquipCombatWeapon();
	
	/*auto InteractComponent = FindComponentByClass<UInteractSphereComponent>();
	
	if (!InteractComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACoopGnomeCharacter::Interact(): No interact component!"));
		return;
	}
	
	bool bInteracted = InteractComponent->Interact();
	
	if (bInteracted)
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("INTERACT SUCCESS")));

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("INTERACT  %hhd "), bInteracted));
	*/
}

void ACoopGnomeCharacter::EquipNextWeapon(const FInputActionValue& Value)
{
	if (!InventoryComponent)
	{
		//UE_LOG(LogTemp, Warning, TEXT("ACoopGnomeCharacter::EquipNextWeapon(): InventoryComponent is not valid!"));
		return;
	}

	for (const auto& Item : InventoryComponent->CurrentItems)
	{
		if (!Item.WeaponClass)
			continue;

		if (!OverlappingWeapon || Item.WeaponClass != OverlappingWeapon.GetClass())
		{
			EquipWeapon(Item.Name);
			return;
		}
	}
}
