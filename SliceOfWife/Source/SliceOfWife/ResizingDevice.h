// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnumsStructs.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ResizingDevice.generated.h"

UCLASS()
class SLICEOFWIFE_API AResizingDevice : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AResizingDevice();

	AActor* objectOnDevice = nullptr;
	bool isActive = false;

	UPROPERTY(BlueprintReadOnly)
	float ActiveTimer = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float WaitTime = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FailTime = 15;

	UPROPERTY(EditAnywhere)
	FVector SnapLocation = { 0, 0, 100 };

	UPROPERTY(EditAnywhere)
	FRotator SnapRotation = { 0, 0, 0 };

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> FailedProduct;

	UPROPERTY(EditAnywhere)
	TArray<FObjectReplacement> ObjectReplacements;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float ImpulseStrength = 500;

	UPROPERTY(EditAnywhere)
	float ImpulseAngle = 60;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	bool DropToDevice(AActor* object);
	bool RemoveFromDevice(AActor* requester = nullptr);

	bool ReplaceObject();
	bool Eject(AActor* towards = nullptr);

	UFUNCTION(BlueprintCallable)
	bool IsOccupied();

	UFUNCTION(BlueprintCallable)
	void UpdateTimer();
};
