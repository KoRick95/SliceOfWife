// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnumsStructs.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AnimatingDevice.generated.h"

UCLASS()
class SLICEOFWIFE_API AAnimatingDevice : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAnimatingDevice();

	class AAssemblingTable* assemblingTable = nullptr;

	UPROPERTY(VisibleAnywhere)
	bool Animated = false;

	UPROPERTY(EditAnywhere)
	TArray<TEnumAsByte<EBodyPartType>> RequiredBodyPartTypes;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool AnimateBody();
};
