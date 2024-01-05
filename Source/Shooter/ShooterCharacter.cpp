// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AShooterCharacter::AShooterCharacter() :
	bInvertPitchAxis(false)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a spring arm (pulls in towards character if there is a collision)
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 300.f;			// The camera follows at this distance behind the character
	SpringArmComponent->bUsePawnControlRotation = true;		// Rotation based on the controller
	SpringArmComponent->SocketOffset = FVector(0.f, 50.f, 50.f);

	// Create the third person camera that is attached to the spring arm
	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName); // Attach camera to spring arm
	ThirdPersonCamera->bUsePawnControlRotation = false;		// Camera does not rotate relative to arm.

	// Rotate the yaw when the controller rotates. For everything else, the controller only affect camera
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false;	// character does not move in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);	// ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();


	/* Setup and Configuration of the Enhanced Input system. It gets the controller,
	 * checks the subsystem and gets the local player and then connect the IMC to the controller. */
	if(const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerContext, 0);
		}
	}
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Calls the EnhancedInput and binds the input actions
	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Jump);
		EnhancedInputComponent->BindAction(PrimaryAttackAction, ETriggerEvent::Started, this, &AShooterCharacter::PrimaryAttack);
		EnhancedInputComponent->BindAction(PrimaryAttackAction, ETriggerEvent::Canceled, this, &AShooterCharacter::PrimaryAttack);
	}

}


/* Movement configuration using the values from the FInputActionValue struct and then calculate it to
 * the MovementInput */
void AShooterCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = GetController()->GetControlRotation();
	const FRotator YawRotation = FRotator(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

/* Configuration for looking with the mouse or right thumbstick where the FInputActionValue calculates into
 * the controller pitch and yaw to adjust the camera position. */
void AShooterCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if(bInvertPitchAxis)
		AddControllerPitchInput(-LookAxisVector.Y);
	else	
		AddControllerPitchInput(LookAxisVector.Y);
	
	AddControllerYawInput(LookAxisVector.X);
}

/** Configuration mainly handled by the parent class */
void AShooterCharacter::Jump()
{
	// TODO: Add and if-statement to check health since that might otherwise bug out when player is dead.
	Super::Jump();
}

void AShooterCharacter::PrimaryAttack(const FInputActionValue& Value)
{
	if(AttackSound) UGameplayStatics::SpawnSoundAtLocation(this, AttackSound, GetActorLocation());
	//if(AttackSound) UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
	//if(AttackSound) UGameplayStatics::PlaySound2D(this, AttackSound);

	if(const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket"))
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());

		if(MuzzleFlash) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);

		// Get current size of the viewport
		FVector2D ViewportSize;
		if(GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize(ViewportSize);
		}

		// Get screen space location of crosshair
		FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
		CrosshairLocation.Y -= 50.f;
		FVector CrosshairWorldPosition;
		FVector CrosshairWorldDirection;

		// Get World position and direction of crosshair
		bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
			UGameplayStatics::GetPlayerController(this, 0),
				CrosshairLocation,
				CrosshairWorldPosition,
				CrosshairWorldDirection
		);

		if(bScreenToWorld)	// was deprojection successful?
		{
			FHitResult ScreenTraceHit;
			const FVector Start = CrosshairWorldPosition;
			const FVector End = CrosshairWorldPosition + CrosshairWorldDirection * 50'000;

			// Set beam end point to line trace endpoint
			FVector BeamEndPoint = End;

			// Trace outward from crosshair world location
			GetWorld()->LineTraceSingleByChannel(
				ScreenTraceHit,
				Start, End,
				ECollisionChannel::ECC_Visibility
			);

			if(ScreenTraceHit.bBlockingHit)		// was there a trace hit?
			{
				// Beam endpoint is now trace hit location
				BeamEndPoint = ScreenTraceHit.Location;
				if(ImpactParticles) UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticles,
					ScreenTraceHit.Location
					);				
			}
			if(BeamParticles)
			{
				UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					BeamParticles,
					SocketTransform
				);
				if(Beam)
				{
					Beam->SetVectorParameter(FName("Target"), BeamEndPoint);
				}
			}
		}

		/*
		FHitResult PrimaryAttackHitResult;
		const FVector Start = SocketTransform.GetLocation();
		const FQuat Rotation = SocketTransform.GetRotation();
		const FVector RotationAxis = Rotation.GetAxisX();
		const FVector End = Start + RotationAxis * 50000.f;

		FVector BeamEndPoint = End;

		GetWorld()->LineTraceSingleByChannel(
			PrimaryAttackHitResult, Start, End,
			ECollisionChannel::ECC_Visibility
		);
		if(PrimaryAttackHitResult.bBlockingHit)
		{	
			BeamEndPoint = PrimaryAttackHitResult.Location;

			if(ImpactParticles)
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, PrimaryAttackHitResult.Location);
		}

		if(BeamParticles)
		{
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(), BeamParticles, SocketTransform
				);
			if(Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEndPoint);
			}
		} */
	}	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && PrimaryAttackMontage)
	{
		AnimInstance->Montage_Play(PrimaryAttackMontage);
		AnimInstance->Montage_JumpToSection(FName("StartPrimaryFire"));
	}
}
