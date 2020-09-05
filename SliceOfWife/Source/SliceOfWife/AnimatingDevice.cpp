// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimatingDevice.h"
#include "AssemblingTable.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAnimatingDevice::AAnimatingDevice()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAnimatingDevice::BeginPlay()
{
	Super::BeginPlay();

	assemblingTable = Cast<AAssemblingTable>(UGameplayStatics::GetActorOfClass(this, AAssemblingTable::StaticClass()));
}

// Called every frame
void AAnimatingDevice::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AAnimatingDevice::AnimateBody()
{
	if (assemblingTable == nullptr)
	{
		return false;
	}



	return false;
}
