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
	

protected:

	/** Handles the player movement */
	void Move(const FInputActionValue& Value);

	/** Handles the camera movement by either the mouse or the right thumbstick */
	void Look(const FInputActionValue& Value);

	/** Handles the player jumping, calls the parent function for the jumping functionality */
	virtual void Jump() override;
	
public:

	/** Returns SpringArmComponent sub-object */
	FORCEINLINE USpringArmComponent* GetSpringArmComponent() const { return SpringArmComponent; }

	/** Returns ThirdPersonCamera sub-object */
	FORCEINLINE UCameraComponent* GetThirdPersonCamera() const { return ThirdPersonCamera; }	

};
