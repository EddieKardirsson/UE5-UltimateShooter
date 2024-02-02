// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

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

		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());

		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		if(ShooterCharacter->GetVelocity().Size() > 0.f)
			LastMovementOffsetYaw = MovementOffsetYaw;

		FString LastOffset = FString::Printf(TEXT("LastMovementOffsetYaw: %f"), LastMovementOffsetYaw);
		FString CurrentVelocity = FString::Printf(TEXT("CurrentVelocity: %f"), ShooterCharacter->GetVelocity().Size());
		if(GEngine) GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::White, LastOffset);
		if(GEngine)	GEngine->AddOnScreenDebugMessage(2, 1.f, FColor::Green, CurrentVelocity);
	}
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// Get the pawn owner, cast it to playable character and initialize
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}
