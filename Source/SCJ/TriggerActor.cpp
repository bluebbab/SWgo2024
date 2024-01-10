// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerActor.h"
#include "Components/BoxComponent.h"
#include "SCJCharacter.h"

// Sets default values
ATriggerActor::ATriggerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->InitBoxExtent(FVector(50, 50, 50));
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ATriggerActor::OnOverlapBegin);
}

// Called when the game starts or when spawned
void ATriggerActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATriggerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATriggerActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ASCJCharacter* Player = Cast<ASCJCharacter>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player is Overlapped"));
		GetWorld()->DestroyActor(ActorToDestroy);
	}
}

