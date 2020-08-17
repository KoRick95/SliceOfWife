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

	AActor* hauntedObject;

	FVector direction;

	float spawnTimer = 0;

	UPROPERTY(EditAnywhere)
	float MoveSpeed = 100;

	UPROPERTY(EditAnywhere)
	float MapEdgeX = 1000;

	UPROPERTY(EditAnywhere)
	float MapEdgeY = 1000;

	UPROPERTY(EditAnywhere)
	bool CanRespawn = true;

	UPROPERTY(EditAnywhere)
	float RespawnTime = 20;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void HoldObject();
	void ReleaseObject();

	void Appear();
	void Disappear();
};
