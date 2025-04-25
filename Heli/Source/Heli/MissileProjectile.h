#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MissileProjectile.generated.h"

class UBoxComponent;
class UNiagaraComponent;
class UProjectileMovementComponent;
class UStaticMeshComponent;

UCLASS()
class AMissileProjectile : public AActor
{
	GENERATED_BODY()

public:
	AMissileProjectile();

	void Prime(float CarrierFwdSpeed);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBurn);

	UPROPERTY(BlueprintAssignable)
	FOnBurn OnBurn;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float Delta) override;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(VisibleAnywhere)
	USceneComponent* RootScene;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* Collision;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* Move;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* Trail;

	UPROPERTY(EditAnywhere, Category = "Rocket|Flight")
	float DropTime = 0.35f;

	UPROPERTY(EditAnywhere, Category = "Rocket|Flight")
	float LaunchBoostFactor = 1.15f;

	UPROPERTY(EditAnywhere, Category = "Rocket|Flight")
	float ThrustAccel = 10000.f;

	UPROPERTY(EditAnywhere, Category = "Rocket|Flight")
	float MaxSpeed = 6000.f;

	UPROPERTY(EditAnywhere, Category = "Rocket|Flight")
	float BurnTime = 3.0f;

private:
	float LifeSeconds = 0.f;
	bool bMotorOn = false;
	bool bBurnActivated = false;
};
