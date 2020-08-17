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

	AActor* heldObject;

	FVector direction;

	UPROPERTY(EditAnywhere)
	float moveSpeed = 100;

	UPROPERTY(EditAnywhere)
	float mapEdgeX = 1000;

	UPROPERTY(EditAnywhere)
	float mapEdgeY = 1000;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void HoldObject(AActor* object);
};
