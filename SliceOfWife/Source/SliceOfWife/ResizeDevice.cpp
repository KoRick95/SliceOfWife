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
	return false;
}

