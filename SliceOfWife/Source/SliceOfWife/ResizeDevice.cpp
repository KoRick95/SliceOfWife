// Fill out your copyright notice in the Description page of Project Settings.

#include "ResizeDevice.h"
#include "BodyPart.h"

// Sets default values
AResizeDevice::AResizeDevice()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AResizeDevice::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AResizeDevice::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AResizeDevice::ReplaceBody(ABodyPart* bodyPart)
{
	for (int i = 0; i < BodyPartReplacements.Num(); ++i)
	{
<<<<<<< HEAD
		if (BodyPartReplacements[i].input.Get() == bodyPart->GetClass())
=======
		if (BodyPartReplacements[i].Input.Get() == bodyPart->GetClass())
>>>>>>> 8a3e1867d55f2d48c87933dcd546baf6743c55f2
		{

		}
	}

	return false;
}

