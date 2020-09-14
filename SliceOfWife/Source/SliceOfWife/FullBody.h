// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnumsStructs.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FullBody.generated.h"

UCLASS()
class SLICEOFWIFE_API AFullBody : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFullBody();

	TArray<class ABodyPart*> bodyParts;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECreatureType> CreatureType;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	int GetBodyPartsCount();

	void AttachBodyPart(ABodyPart* bodyPart);
	bool DetachBodyPart(ABodyPart* bodyPart);

	UFUNCTION(BlueprintCallable)
	int CountCreatureTypeVariation();
};
