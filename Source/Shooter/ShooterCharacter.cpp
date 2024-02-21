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

// Sets default values in the initializer list
AShooterCharacter::AShooterCharacter() :
	LookRate(45.f),
	HipLookRate(90.f),
	AimingLookRate(20.f),
	bInvertPitchAxis(false),
	bAiming(false),
	CameraDefaultFOV(0.f),	// set in BeginPlay
	CameraZoomedFOV(35.f),
	CameraCurrentFOV(0.f),	// set in BeginPlay
	ZoomInterpSpeed(20.f),
	CrosshairSpreadMultiplier(0.f),
	CrosshairVelocityFactor(0.f),
	CrosshairInAirFactor(0.f),
	CrosshairAimFactor(0.f),
	CrosshairShootingFactor(0.f),
	bPrimaryAttackButtonPressed(false),
	bShouldFire(true),
	AutomaticFireRate(0.1f),
	bToggleAutomaticPrimaryAttack(false),
	ShootTimeDuration(0.05f),
	bFiringBullet(false)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a spring arm (pulls in towards character if there is a collision)
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 180.f;			// The camera follows at this distance behind the character
	SpringArmComponent->bUsePawnControlRotation = true;		// Rotation based on the controller
	SpringArmComponent->SocketOffset = FVector(0.f, 50.f, 70.f);

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

	if(ThirdPersonCamera)
	{
		CameraDefaultFOV = GetThirdPersonCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	

	// Set current camera field of view
	InterpolateCameraFOV(DeltaTime);

	// Change Look sensitivity depending on aiming or not
	SetLookRate();

	// Calculate crosshair spread multiplier
	CalculateCrosshairSpread(DeltaTime);
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
		//EnhancedInputComponent->BindAction(PrimaryAttackAction, ETriggerEvent::Started, this, &AShooterCharacter::PrimaryAttack);
		//EnhancedInputComponent->BindAction(PrimaryAttackAction, ETriggerEvent::Canceled, this, &AShooterCharacter::PrimaryAttack);
		EnhancedInputComponent->BindAction(PrimaryAttackAction, ETriggerEvent::Started, this, &AShooterCharacter::PrimaryAttackButtonPressed);
		EnhancedInputComponent->BindAction(PrimaryAttackAction, ETriggerEvent::Completed, this, &AShooterCharacter::PrimaryAttackButtonReleased);		
		EnhancedInputComponent->BindAction(ToggleAutomaticPrimaryAttackAction, ETriggerEvent::Started, this, &AShooterCharacter::ToggleAutomaticPrimaryAttack);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AShooterCharacter::Aim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AShooterCharacter::StopAim);
	}

}


/* Movement configuration using the values from the FInputActionValue struct and then calculate it to
 * the MovementInput */
void AShooterCharacter::Move(const FInputActionValue& Value)
{
	const float MovementSpeed = GetMovementComponent()->GetMaxSpeed();
	const FVector2D MovementVector = Value.Get<FVector2D>() * GetWorld()->GetDeltaSeconds() * MovementSpeed;

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
	const FVector2D LookAxisVector = Value.Get<FVector2D>() * GetWorld()->GetDeltaSeconds() * LookRate;

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

		FVector BeamEnd;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamEnd);
		if(bBeamEnd)
		{
			if(ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(), ImpactParticles, BeamEnd);
			}

			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(), BeamParticles, SocketTransform);
			
			if(Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}	
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && PrimaryAttackMontage)
	{
		AnimInstance->Montage_Play(PrimaryAttackMontage);
		AnimInstance->Montage_JumpToSection(FName("StartPrimaryFire"));
	}

	// Start bullet fire timer for crosshair
	StartCrosshairBulletFire();
}

bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation)
{
	// Get current size of the viewport
	FVector2D ViewportSize;
	if(GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// Get screen space location of crosshair
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
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
		OutBeamLocation = End;

		// Trace outward from crosshair world location
		GetWorld()->LineTraceSingleByChannel(
			ScreenTraceHit,
			Start, End,
			ECollisionChannel::ECC_Visibility
		);

		if(ScreenTraceHit.bBlockingHit)		// was there a trace hit?
		{
			// Beam endpoint is now trace hit location
			OutBeamLocation = ScreenTraceHit.Location;					
		}

		// Perform a second trace, this time from the gun barrel
		FHitResult WeaponTraceHit;
		const FVector WeaponTraceStart{ MuzzleSocketLocation};
		const FVector WeaponTraceEnd{ OutBeamLocation };
		GetWorld()->LineTraceSingleByChannel(
			WeaponTraceHit,
			WeaponTraceStart,
			WeaponTraceEnd,
			ECollisionChannel::ECC_Visibility
		);

		if(WeaponTraceHit.bBlockingHit)	// Object between barrel and BeamEndPoint?
			OutBeamLocation = WeaponTraceHit.Location;

		return true;
	}
	
	return false;
}

void AShooterCharacter::Aim(const FInputActionValue& Value)
{
	bAiming = true;

	// Add debug logs
	UE_LOG(LogTemp, Warning, TEXT("Aim called. bAiming: %d"), bAiming);
}

void AShooterCharacter::StopAim(const FInputActionValue& Value)
{
	bAiming = false;

	// Add a debug log
	UE_LOG(LogTemp, Warning, TEXT("StopAim called"));
	UE_LOG(LogTemp, Warning, TEXT("StopAim called. bAiming: %d"), bAiming);
}

void AShooterCharacter::InterpolateCameraFOV(float DeltaTime)
{
	if(bAiming)
	{
		// Interpolate to zoomed FOV
		CameraCurrentFOV  = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomInterpSpeed);		
	}
	else
	{
		// Interpolate to default FOV
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);
	}
	GetThirdPersonCamera()->SetFieldOfView(CameraCurrentFOV);
}

void AShooterCharacter::SetLookRate()
{
	if(bAiming)
		LookRate = AimingLookRate;
	else
		LookRate = HipLookRate;
}

void AShooterCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	FVector2D WalkSpeedRange{0.f, 600.f };
	FVector2D VelocityMultiplierRange{ 0.f, 1.f };
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;

	// Calculate crosshair velocity factor
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(
		WalkSpeedRange,
		VelocityMultiplierRange,
		Velocity.Size());

	// Calculate crosshair in air factor
	if(GetCharacterMovement()->IsFalling())	// is in air?
	{
		// Spread the crosshair slowly while in air
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
	}
	else	// Character is on the ground
	{
		// Shrink the crosshair rapidly while on the ground
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
	}

	// Calculate crosshair aim factor
	if(bAiming)	// Are we aiming?
	{
		// Shrink crosshair a small amount very quickly
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.6f, DeltaTime, 30.f);
	}
	else		// Not aiming
	{
		// Spread crosshair back to normal very quickly
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
	}
	
	// True 0.05 seconds after firing
	if(bFiringBullet)
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.3f, DeltaTime, 60.f);
	}
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 60.f);
	}
	
	CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor + CrosshairInAirFactor - CrosshairAimFactor + CrosshairShootingFactor;
}

void AShooterCharacter::PrimaryAttackButtonPressed(const FInputActionValue& Value)
{
	bPrimaryAttackButtonPressed = true;
	StartPrimaryAttackTimer(Value);
}

void AShooterCharacter::PrimaryAttackButtonReleased(const FInputActionValue& Value)
{
	bPrimaryAttackButtonPressed = false;
}

void AShooterCharacter::StartPrimaryAttackTimer(const FInputActionValue& Value)
{
	// Check if rifle is set to automatic
	if(bToggleAutomaticPrimaryAttack)	// if true -> start the automatic firing sequence
	{
		if(bShouldFire)
		{
			PrimaryAttack(Value);
			bShouldFire = false;
			FTimerDelegate TimerDel;
			TimerDel.BindUFunction(this, FName(TEXT("AutoFireReset")), Value); // Check if this works
			GetWorldTimerManager().SetTimer(AutoFireTimer, TimerDel, AutomaticFireRate, false);
		}	
	}
	else	// false? -> do the original single shot
	{
		PrimaryAttack(Value);
	}
}

void AShooterCharacter::AutoFireReset(const FInputActionValue& Value)
{	
	bShouldFire = true;
	if(bPrimaryAttackButtonPressed)
	{
		StartPrimaryAttackTimer(Value);
	}
}

void AShooterCharacter::ToggleAutomaticPrimaryAttack(const FInputActionValue& Value)
{
	if(bToggleAutomaticPrimaryAttack)
		bToggleAutomaticPrimaryAttack = false;
	else
		bToggleAutomaticPrimaryAttack = true;

	FString ToggleValue = FString::Printf(TEXT("bToggleAutomaticPrimaryAttack: %s"), bToggleAutomaticPrimaryAttack? TEXT("true") : TEXT("false"));
	if(GEngine) GEngine->AddOnScreenDebugMessage(3, 2.f, FColor::Orange, ToggleValue);
}

void AShooterCharacter::StartCrosshairBulletFire()
{
	bFiringBullet = true;

	GetWorldTimerManager().SetTimer(
		CrosshairShootTimer,
		this,
		&AShooterCharacter::FinishCrosshairBulletFire,
		ShootTimeDuration
		);
}

void AShooterCharacter::FinishCrosshairBulletFire()
{
bFiringBullet = false;
}

float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}
