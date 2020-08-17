// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SoulSpawner.generated.h"

UCLASS()
class SLICEOFWIFE_API ASoulSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASoulSpawner();

	class ASoul* soul = nullptr;
	AActor* hauntedObject = nullptr;


	float spawnTimer = 0;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASoul> SoulBP = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SpawnSoul();
};
