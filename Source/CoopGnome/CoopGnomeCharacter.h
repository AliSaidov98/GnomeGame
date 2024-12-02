// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "Base/CharacterBase.h"
#include "Logging/LogMacros.h"
#include "CoopGnomeCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class AWeaponBase;
class UCombatComponent;
class AWeapon;
class UInventoryComponent;


DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftGame);

UCLASS(config=Game)
class ACoopGnomeCharacter : public ACharacterBase
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	
	/** Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

	/** Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

	
	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipNextWeaponAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCombatComponent* Combat;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	ACoopGnomeCharacter();

	UFUNCTION(Server, Reliable)
	void ServerLeaveGame();

	FOnLeftGame OnLeftGame;

	virtual void PostInitializeComponents() override;
	
protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			
	/** Called for looking input */
	void Attack(const FInputActionValue& Value);
			
	/** Called for interact input */
	void Interact(const FInputActionValue& Value);
	
	/** Called for interact input */
	void EquipNextWeapon(const FInputActionValue& Value);
	
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay() override;;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AWeaponBase> DefaultWeaponClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName WeaponAttachSocketName;
	
	void OnPlayerStateInitialized();

public:
	
	UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly, Category = "Weapon")
	bool IsEquippedWeapon;

	UPROPERTY(BlueprintReadOnly, Replicated)
	FRotator ControlRotation;

	UPROPERTY(BlueprintReadOnly, Replicated)
	FRotator ActorRotation;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }

	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	AWeapon* GetEquippedWeapon();

	UFUNCTION(Server, Reliable)
	void ServerInteractPressed();

	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayDeathMontage();
	void PlaySwapMontage();

	bool bFinishedSwapping = false;


private:
	void SetupInventory();
	UFUNCTION()
	void EquipWeapon(FString WeaponName);
	UFUNCTION()
	void UnequipWeapon(FString WeaponName);

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;
	
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	TObjectPtr<AWeaponBase> EquippedWeapon;

	UInventoryComponent* InventoryComponent;
	
	UPROPERTY()
	class ACoopGnomePlayerState* CoopGnomePlayerState;

	UPROPERTY()
	class ACoopGnomeGameMode* CoopGnomeGameMode;

public:
	TSharedPtr<class IOnlineSession, ESPMode::ThreadSafe> OnlineSessionInterface;
};

