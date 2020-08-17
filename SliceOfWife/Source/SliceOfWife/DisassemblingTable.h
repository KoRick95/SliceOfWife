// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DisassemblingTable.generated.h"

UCLASS()
class SLICEOFWIFE_API ADisassemblingTable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADisassemblingTable();

	AActor* bodyOnTable = nullptr;
	TArray<AActor*> bodyParts;

	float charge = 0;

	TArray<float> soulSpawnTimers;

	UPROPERTY(EditAnywhere, Category = "Snapping")
	FName TagToCheck = "FullCharacter";

	UPROPERTY(EditAnywhere, Category = "Snapping")
	FVector SnapPosition = { 0, 0, 150 };

	UPROPERTY(EditAnywhere, Category = "Snapping")
	FRotator SnapRotation = { -90, 0, 0 };

	UPROPERTY(EditAnywhere)
	float ChargeRate = 1;

	UPROPERTY(EditAnywhere)
	float MaxCharge = 5;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ASoul> SoulBP;

	UPROPERTY(EditAnywhere)
	float SoulSpawnDelayMin = 1;

	UPROPERTY(EditAnywhere)
	float SoulSpawnDelayMax = 3;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool RemoveFromTable();
	bool DropToTable(AActor* body);

	void Charge();
};
