// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "UObject/ObjectPtr.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	/** The "tick" function */
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

	/** The initializer function overridden from the parent class UAnimInstance */
	virtual void NativeInitializeAnimation() override;

private:

	/** The cached reference for the playable character to link animation data between C++ and BP */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta =(AllowPrivateAccess = "true"))
	TObjectPtr<class AShooterCharacter> ShooterCharacter;

	/** The speed of the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta =(AllowPrivateAccess = "true"))
	float Speed;
	/** Checks if the character is in air */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta =(AllowPrivateAccess = "true"))
	bool bIsInAir;

	/** Checks if character is accelerating */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta =(AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	/** Offset yaw used for strafing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta =(AllowPrivateAccess = "true"))
	float MovementOffsetYaw;

	/** Offset yaw the frame before stop moving */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta =(AllowPrivateAccess = "true"))
	float LastMovementOffsetYaw;
	
};
