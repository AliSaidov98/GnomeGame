

#include "ProjectileBullet.h"

#include "BulletMovementComponent.h"
#include "CoopGnome/CoopGnomeCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


AProjectileBullet::AProjectileBullet()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	
	BulletMovementComponent = CreateDefaultSubobject<UBulletMovementComponent>(TEXT("Bullet Movement Component"));
	BulletMovementComponent->bRotationFollowsVelocity = true;
	BulletMovementComponent->SetIsReplicated(true);
}

void AProjectileBullet::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{

	if(OtherActor == GetOwner()) return;
	
	ACoopGnomeCharacter* OwnerCharacter = Cast<ACoopGnomeCharacter>(GetOwner());

	if(OwnerCharacter)
	{
		AController* OwnerController = OwnerCharacter->Controller;

		if(OwnerController)
		{
			UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
		}
	}
	
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

void AProjectileBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

