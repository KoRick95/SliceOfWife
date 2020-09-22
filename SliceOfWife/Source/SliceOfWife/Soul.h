// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Soul.generated.h"

UCLASS()
class SLICEOFWIFE_API ASoul : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASoul();

	AActor* possession;

	FVector direction;

	bool spawnTimerOn = true;
	float spawnTimer = 0;

	bool hasSpawned = false;

	UPROPERTY(EditAnywhere)
	float FloatHeight = 150;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float MoveSpeed = 100;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float MapEdgeX = 1000;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float MapEdgeY = 1000;

	UPROPERTY(EditAnywhere)
	bool DelaySpawn = true;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", EditCondition = "DelaySpawn"))
	float SpawnDelayMin = 1;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", EditCondition = "DelaySpawn"))
	float SpawnDelayMax = 3;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	float SpawnChance = 0.5f;

	UPROPERTY(EditAnywhere)
	bool CanRespawn = true;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1", EditCondition = "CanRespawn"))
	float RespawnTime = 20;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector GetRandomDirection();

	void HoldObject();
	void ReleaseObject();

	void Spawn();
	void Despawn();
};
