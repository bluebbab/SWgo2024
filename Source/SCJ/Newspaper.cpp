// Fill out your copyright notice in the Description page of Project Settings.


#include "Newspaper.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Blueprint/UserWidget.h"

// Sets default values
ANewspaper::ANewspaper()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(Mesh);
}

// Called when the game starts or when spawned
void ANewspaper::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANewspaper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANewspaper::Interact()
{
}
