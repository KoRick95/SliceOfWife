// Fill out your copyright notice in the Description page of Project Settings.


#include "FullCharacter.h"
#include "BodyPart.h"
#include "Engine/World.h"

// Sets default values
AFullCharacter::AFullCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Tags.Add("Pickup");
	Tags.Add("FullCharacter");
}

// Called when the game starts or when spawned
void AFullCharacter::BeginPlay()
{
	Super::BeginPlay();	

	if (AutoSpawnBodyParts)
	{
		for (int i = 0; i < BodyParts.Num(); ++i)
		{
			UClass* uClass = BodyParts[i].Get();
			FTransform transform = this->GetActorTransform();
			FActorSpawnParameters spawnParams;

			GetWorld()->SpawnActor(uClass, &transform, spawnParams);
		}
	}
}

// Called every frame
void AFullCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AFullCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

