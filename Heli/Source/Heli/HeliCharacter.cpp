#include "HeliCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "MissileProjectile.h"

AHeliCharacter::AHeliCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(60.f, 120.f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	auto* Move = GetCharacterMovement();
	Move->SetMovementMode(MOVE_Flying);
	Move->GravityScale = 0.f;
	Move->MaxFlySpeed = 2200.f;
	Move->BrakingDecelerationFlying = 300.f;
	Move->bOrientRotationToMovement = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->bUsePawnControlRotation = !bDetachCamera;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = !bDetachCamera;

	LauncherRefLeft = CreateDefaultSubobject<UArrowComponent>(TEXT("LauncherRefLeft"));
	LauncherRefLeft->SetupAttachment(RootComponent);

	LauncherRefRight = CreateDefaultSubobject<UArrowComponent>(TEXT("LauncherRefRight"));
	LauncherRefRight->SetupAttachment(RootComponent);
}

void AHeliCharacter::BeginPlay()
{
	Super::BeginPlay();

	CameraBoom->bUsePawnControlRotation = !bDetachCamera;
	FollowCamera->bUsePawnControlRotation = !bDetachCamera;

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (auto* SubSys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			SubSys->AddMappingContext(HeliMappingContext, 0);
		}
	}

	auto* Move = GetCharacterMovement();
	if (!Move->IsFlying())
	{
		Move->SetMovementMode(MOVE_Flying);
		Move->DefaultLandMovementMode = MOVE_Flying;
		Move->DefaultWaterMovementMode = MOVE_Flying;
	}
}

void AHeliCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (auto* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(DirectionalAction, ETriggerEvent::Triggered, this, &AHeliCharacter::Directional);
		EIC->BindAction(DirectionalAction, ETriggerEvent::Completed, this, &AHeliCharacter::Directional);

		EIC->BindAction(CollectiveAction, ETriggerEvent::Triggered, this, &AHeliCharacter::Collective);
		EIC->BindAction(CollectiveAction, ETriggerEvent::Completed, this, &AHeliCharacter::Collective);

		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &AHeliCharacter::Look);

		EIC->BindAction(FireAction, ETriggerEvent::Completed, this, &AHeliCharacter::FireMissile);
	}
}

void AHeliCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentYawRate = FMath::FInterpTo(CurrentYawRate, DesiredYawRate, DeltaTime, YawInterpSpeed);
	AddControllerYawInput(CurrentYawRate * DeltaTime);

	const float TargetPitch = FMath::Clamp(-ForwardInput * MaxPitchDeg, -MaxPitchDeg, MaxPitchDeg);
	const float TargetRoll = FMath::Clamp(RightInput * MaxRollDeg, -MaxRollDeg, MaxRollDeg);
	const FRotator Current = GetActorRotation();
	const FRotator Desired(TargetPitch, Current.Yaw, TargetRoll);

	SetActorRotation(FMath::RInterpTo(Current, Desired, DeltaTime, TiltInterpSpeed));
}

void AHeliCharacter::Directional(const FInputActionValue& Val)
{
	const FVector2D Axis = Val.Get<FVector2D>();
	ForwardInput = Axis.Y;
	RightInput = Axis.X;

	if (Controller)
	{
		const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
		AddMovementInput(FRotationMatrix(YawRot).GetUnitAxis(EAxis::X), ForwardInput);
		AddMovementInput(FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y), RightInput);
	}

	const bool bTurning = FMath::Abs(RightInput) > KINDA_SMALL_NUMBER && FMath::Abs(ForwardInput) > KINDA_SMALL_NUMBER;
	DesiredYawRate = bTurning ? RightInput * StickYawRateDeg : 0.f;
}

void AHeliCharacter::Look(const FInputActionValue& Val)
{
	if (!bDetachCamera) return;

	const FVector2D Axis = Val.Get<FVector2D>();
	CamYaw += Axis.X * LookRate;
	CamPitch = FMath::Clamp(CamPitch - Axis.Y * LookRate, -MaxCameraPitch, MaxCameraPitch);
	CameraBoom->SetRelativeRotation(FRotator(CamPitch, CamYaw, 0.f));
}

void AHeliCharacter::Collective(const FInputActionValue& Val)
{
	UpInput = Val.Get<float>();
	AddMovementInput(FVector::UpVector, UpInput);
}

void AHeliCharacter::FireMissile(const FInputActionValue& Val)
{
	if (!MissileClass || !LauncherRefLeft || !LauncherRefRight) return;

	const FTransform SpawnTM = bLaunchLeft ? LauncherRefLeft->GetComponentTransform() : LauncherRefRight->GetComponentTransform();
	bLaunchLeft = !bLaunchLeft;

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = this;

	AMissileProjectile* Rocket = GetWorld()->SpawnActor<AMissileProjectile>(MissileClass, SpawnTM, Params);
	if (!Rocket) return;

	const FVector Fwd = SpawnTM.GetRotation().GetForwardVector();
	const float Speed = FVector::DotProduct(GetVelocity(), Fwd);
	Rocket->Prime(FMath::Max(Speed, 0.f));
}
