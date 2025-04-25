#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HeliCharacter.generated.h"

class AMissileProjectile;
class UArrowComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
struct FInputActionValue;

UCLASS(config = Game)
class AHeliCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AHeliCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent* LauncherRefLeft = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent* LauncherRefRight = nullptr;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void FireMissile(const FInputActionValue& Val);

	UPROPERTY(EditAnywhere, Category = "Flight")
	float MaxPitchDeg = 15.f;

	UPROPERTY(EditAnywhere, Category = "Flight")
	float MaxRollDeg = 20.f;

	UPROPERTY(EditAnywhere, Category = "Flight")
	float TiltInterpSpeed = 3.f;

	UPROPERTY(EditAnywhere, Category = "Flight")
	float LookRate = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Flight")
	float MaxOrbitAcquireDist = 6000.f;

	UPROPERTY(EditAnywhere, Category = "Flight|StickControl")
	float StickYawRateDeg = 30.f;

	UPROPERTY(EditAnywhere, Category = "Flight|StickControl")
	float YawInterpSpeed = 3.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float MaxCameraPitch = 80.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	bool bDetachCamera = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TSubclassOf<AMissileProjectile> MissileClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* HeliMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* DirectionalAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* CollectiveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* OrbitAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* FireAction;

private:
	void Directional(const FInputActionValue& Value);
	void Collective(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	float ForwardInput = 0.f;
	float RightInput = 0.f;
	float UpInput = 0.f;

	bool bLaunchLeft = false;

	float CamYaw = 0.f;
	float CamPitch = 0.f;

	float DesiredYawRate = 0.f;
	float CurrentYawRate = 0.f;
};
