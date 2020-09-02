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

	class AFullBody* bodyOnTable = nullptr;

	float charge = 0;

	UPROPERTY(EditAnywhere, Category = "Snapping")
	FVector SnapPosition = { 0, 0, 150 };

	UPROPERTY(EditAnywhere, Category = "Snapping")
	FRotator SnapRotation = { -90, 0, 0 };

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float ChargeRate = 1;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float MaxCharge = 5;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ASoul> SoulBP;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool DropToTable(AActor* body);
	bool RemoveFromTable();

	void Charge();
	void Disassemble();
};