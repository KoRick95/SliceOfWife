// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnumsStructs.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ResizeDevice.generated.h"

UCLASS()
class SLICEOFWIFE_API AResizeDevice : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AResizeDevice();

	UPROPERTY(EditAnywhere)
	TArray<FBodyPartReplacement> BodyPartReplacements;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	bool ReplaceBody(class ABodyPart* bodyPart);
};
