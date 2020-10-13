// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnumsStructs.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AssemblingSpot.generated.h"


UCLASS()
class SLICEOFWIFE_API AAssemblingSpot : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAssemblingSpot();

	class AAssemblingTable* table = nullptr;

	class ABodyPart* assignedBodyPart = nullptr;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EBodyPartType> BodyPartType;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool SetBodyPart(ABodyPart* aBodyPart);

	bool DropToTable(AActor* object);
	bool RemoveFromTable(ABodyPart* aBodyPart);

	bool BeginSewing();
	void AssembleBodyPart();

	UFUNCTION(BlueprintCallable)
	bool IsOccupied();

	UFUNCTION(BlueprintCallable)
	bool CanDropToTable(AActor* object);
};