// The class for body storage items

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BodyStorage.generated.h"

UCLASS()
class SLICEOFWIFE_API ABodyStorage : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABodyStorage();

	TArray<AActor*> currentBodies;

	UPROPERTY(EditAnywhere)
	bool bPermamentLimit = false;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	int MaxBodyCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<class ACreature>> CreaturePool;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	AActor* TakeBody();
};
