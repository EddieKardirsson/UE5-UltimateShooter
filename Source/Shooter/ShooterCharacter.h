// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UObject/ObjectPtr.h"
#include "InputActionValue.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	/** Spring Arm positioning camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> SpringArmComponent;

	/** Camera that follows the character and is attached to the spring arm */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> ThirdPersonCamera;

	/** Base looking rate for mouse and right thumbsticks, degree per (delta)second */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float LookRate;

	/** Look rate while not aiming */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float HipLookRate;

	/** Look rate while aiming */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float AimingLookRate;

	/** This property is to invert the values for the camera pitch (i.e. Pilot mode) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	bool bInvertPitchAxis;
	
	/** InputMappingContext cache that handles the keybindings for corresponding InputActions.
	 * Add the InputMappingContext blueprint IMC_ShooterCharacter on this slot */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput | Context", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> PlayerContext;

	/** InputAction cache for the movement. Add IA_Move to this slot */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput | Movement", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;

	/** InputAction cache for the camera movement with mouse or right thumbstick */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput | Movement", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

	/** InputAction cache for jumping */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput | Movement", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;

	/** InputAction cache for primary attack */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput | Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> PrimaryAttackAction;

	/** Handles aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput | Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> AimAction;

	/** Toggle between automatic or single shot */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput | Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ToggleAutomaticPrimaryAttackAction;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput | Actions", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SelectAction;

	/** Randomized gunshot MetaSound, default blueprint should be MS_Shot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio | Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> AttackSound;

	/** Flash spawned at BarrelSocket */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX | Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UParticleSystem> MuzzleFlash;

	/** Montage for primary attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations | Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> PrimaryAttackMontage;
	
	/** Particles spawned upon bullet impact */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX | Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UParticleSystem> ImpactParticles;

	/** Smoke trail for bullets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX | Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UParticleSystem> BeamParticles;

	/** Check if the player is aiming or not */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD Elements | Combat", meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	/** Default Camera field of view value */
	float CameraDefaultFOV;

	/** Field of view value for when zoomed in */
	float CameraZoomedFOV;

	/** Current field of view this fram */
	float CameraCurrentFOV;

	/** Interpolation speed for zooming when aiming */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Elements | Combat", meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;

	/** Determines the spread of the crosshair  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD Elements | Crosshair", meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;

	/** Velocity component for crosshair spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD Elements | Crosshair", meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor;

	/** In air component for crosshair spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD Elements | Crosshair", meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;

	/** Aim component for crosshair spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD Elements | Crosshair", meta = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor;

	/** Shooting component for crosshair spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD Elements | Crosshair", meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor;

	/** Left mouse button or right console trigger pressed */
	bool bPrimaryAttackButtonPressed;

	/** True when we can fire, false when waiting for the timer */
	bool bShouldFire;

	/** Rate of automatic gunfire */
	float AutomaticFireRate;

	/** Toggle automatic on and off */
	bool bToggleAutomaticPrimaryAttack;

	/** Set a timer between gunshots */
	FTimerHandle AutoFireTimer;

	float ShootTimeDuration;
	bool bFiringBullet;
	FTimerHandle CrosshairShootTimer;

	/** True if we should trace every frame for items */
	bool bShouldTraceForItems;

	/** Number of overlapped AItems */
	int8 OverlappedItemCount;

	/** The AItem we hit last frame */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class AItem> TraceHitItemLastFrame;

	/** Currently equipped weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class AWeapon> EquippedWeapon;

	/** Set this in Blueprints for the default weapon class */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;

	/** The ite currently hit by our trace in TraceForItems (could be null) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,  Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AItem> TraceHitItem;

protected:

	/** Handles the player movement */
	void Move(const FInputActionValue& Value);

	/** Handles the camera movement by either the mouse or the right thumbstick */
	void Look(const FInputActionValue& Value);

	/** Handles the player jumping, calls the parent function for the jumping functionality */
	virtual void Jump() override;

	/** Called when the primary attack button is pressed */
	void PrimaryAttack(const FInputActionValue& Value);

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

	/** Set bAiming to true or false */
	void Aim(const FInputActionValue& Value);
	void StopAim(const FInputActionValue& Value);

	void InterpolateCameraFOV(float DeltaTime);
	void SetLookRate();

	void CalculateCrosshairSpread(float DeltaTime);

	void PrimaryAttackButtonPressed(const FInputActionValue& Value);
	void PrimaryAttackButtonReleased(const FInputActionValue& Value);

	void StartPrimaryAttackTimer(const FInputActionValue& Value);

	UFUNCTION()
	void AutoFireReset(const FInputActionValue& Value);

	/** Press the toggle button to switch between single shots or automatic fire */
	void ToggleAutomaticPrimaryAttack(const FInputActionValue& Value);
	
	void StartCrosshairBulletFire();

	UFUNCTION()
	void FinishCrosshairBulletFire();

	/** Linetrace for items under the crosshair */
	bool TraceUnderCrosshair(FHitResult& OutHitResult, FVector& OutHitLocation);

	/** Trace for items if OverlappedItemCount > 0 */
	void TraceForItems();

	/** Spawns a default weapon and equips it */
	AWeapon* SpawnDefaultWeapon();

	/** Takes a weapon and attaches it to the mesh */
	void EquipWeapon(AWeapon* WeaponToEquip);

	/** Detach weapon and let it fall to the ground */
	void DropWeapon();


	void SelectButtonPressed(const FInputActionValue& Value);
	void SelectButtonReleased(const FInputActionValue& Value);

	/** Drops currently equipped Weapon and equips TaceHitItem */
	void SwapWeapon(AWeapon* WeaponToSwap);
	
public:

	/** Returns SpringArmComponent sub-object */
	FORCEINLINE USpringArmComponent* GetSpringArmComponent() const { return SpringArmComponent; }

	/** Returns ThirdPersonCamera sub-object */
	FORCEINLINE UCameraComponent* GetThirdPersonCamera() const { return ThirdPersonCamera; }

	FORCEINLINE bool GetAiming() const { return bAiming; }

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }

	/** Adds/Subtracts to/frokm OverlappedItemCount and updates bShouldTraceForItems */
	void IncrementOverlappedItemCount(int8 Amount);

};
