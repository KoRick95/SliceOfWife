// Fill out your copyright notice in the Description page of Project Settings.

#include "EnumsStructs.h"
#include "ResizeDevice.h"
#include "BodyPart.h"
#include "Engine.h"
#include "Engine/World.h"

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
		if (BodyPartReplacements[i].Input.Get() == bodyPart->GetClass())
		{
			UClass* uClass = BodyPartReplacements[i].Output.Get();
			FTransform transform = bodyPart->GetActorTransform();
			GetWorld()->SpawnActor(uClass, &transform);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Replaced body woo!")));
			return true;
		}
	}

	return false;
}

