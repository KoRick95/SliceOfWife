// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnumsStructs.h"
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

	class ACreature* attachedBody = nullptr;

	class USkeletalMeshComponent* skeletalMeshComponent = nullptr;

	FBodyPartMesh* currentMesh = nullptr;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECreatureType> OriginalCreatureType;

	UPROPERTY(EditAnywhere)
	TArray<FBodyPartMesh> BodyPartMeshes;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AAccessory> AccessoryBlueprint;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float GetMeshRadius();
	FVector GetMeshRelativeLocation();
	TArray<EBodyPartType> GetCurrentMeshTypes();

	void SetPhysicsState(bool state);

	bool IsAttachedToBody();
	bool HasMeshOfType(EBodyPartType type, bool switchMesh = false);
	bool SwitchMesh(int index = 0);

	bool AttachToBody(ACreature* fullBody);
	bool DetachFromBody();
};