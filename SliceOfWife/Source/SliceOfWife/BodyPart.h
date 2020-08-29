// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Enums.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BodyPart.generated.h"

UCLASS()
class SLICEOFWIFE_API ABodyPart : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABodyPart();

	class AFullBody* attachedBody = nullptr;

	class USkeletalMesh* skeletalMesh = nullptr;

	UPROPERTY(EditAnywhere)
	TArray<TEnumAsByte<EBodyPartType>> BodyPartTypes;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector GetMeshRelativeLocation();
	float GetMeshRadius();

	bool CheckForType(EBodyPartType type);

	bool AttachToBody(AFullBody* fullBody);
	bool DetachFromBody();
};