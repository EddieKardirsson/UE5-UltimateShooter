// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if(!ShooterCharacter)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}
	if(ShooterCharacter)
	{
		// Get the lateral speed of the character from Velocity
		FVector Velocity = ShooterCharacter->GetVelocity();
		Velocity.Z = 0;
		Speed = Velocity.Size();

		// Is the character in the air?
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		// Is the character accelerating?
		if(ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0)
			bIsAccelerating = true;
		else
			bIsAccelerating = false;
	}
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// Get the pawn owner, cast it to playable character and initialize
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}
