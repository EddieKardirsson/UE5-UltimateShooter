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

	/** Set bAiming to true */
	void Aim(const FInputActionValue& Value);

	void StopAim(const FInputActionValue& Value);

	void InterpolateCameraFOV(float DeltaTime);
	
public:

	/** Returns SpringArmComponent sub-object */
	FORCEINLINE USpringArmComponent* GetSpringArmComponent() const { return SpringArmComponent; }

	/** Returns ThirdPersonCamera sub-object */
	FORCEINLINE UCameraComponent* GetThirdPersonCamera() const { return ThirdPersonCamera; }

	FORCEINLINE bool GetAiming() const { return bAiming; }

};
