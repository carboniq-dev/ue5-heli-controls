#include "MissileProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"

AMissileProjectile::AMissileProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootScene;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionProfileName("NoCollision");
	Mesh->SetupAttachment(RootComponent);

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->InitBoxExtent(FVector(100.f));
	Collision->SetCollisionProfileName(TEXT("Projectile"));
	Collision->SetGenerateOverlapEvents(true);
	Collision->SetNotifyRigidBodyCollision(true);
	Collision->BodyInstance.bUseCCD = true;
	Collision->SetupAttachment(RootComponent);

	Move = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Move"));
	Move->InitialSpeed = 0.f;
	Move->MaxSpeed = MaxSpeed;
	Move->ProjectileGravityScale = 1.f;
	Move->bRotationFollowsVelocity = true;

	Trail = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Trail"));
	Trail->SetupAttachment(RootComponent);
	Trail->bAutoActivate = false;
}

void AMissileProjectile::BeginPlay()
{
	Super::BeginPlay();

	Collision->OnComponentBeginOverlap.AddDynamic(this, &AMissileProjectile::OnOverlap);
	Collision->OnComponentHit.AddDynamic(this, &AMissileProjectile::OnHit);
}

void AMissileProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	LifeSeconds += DeltaTime;

	if (!bMotorOn && LifeSeconds >= DropTime)
	{
		bMotorOn = true;
		Trail->Activate();

		Move->ProjectileGravityScale = 0.f;
		Move->Velocity = GetActorForwardVector() * 100.f;

		Mesh->SetWorldRotation(Move->Velocity.ToOrientationQuat());
	}

	if (bMotorOn && LifeSeconds <= DropTime + BurnTime)
	{
		if (!bBurnActivated)
			OnBurn.Broadcast();

		bBurnActivated = true;
		Move->Velocity += GetActorForwardVector() * ThrustAccel * DeltaTime;
		Move->Velocity = Move->Velocity.GetClampedToMaxSize(MaxSpeed);
	}
}

void AMissileProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// TODO: Handle overlap
}

void AMissileProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	// TODO: Handle hit
}

void AMissileProjectile::Prime(float CarrierFwdSpeed)
{
	const float Start = FMath::Max(CarrierFwdSpeed * LaunchBoostFactor, 1500.f);
	const float Target = Start * 3.f;

	Move->Velocity = GetActorForwardVector() * Start;
	Move->MaxSpeed = FMath::Max(Move->MaxSpeed, Target);
}
