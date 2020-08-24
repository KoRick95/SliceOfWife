// The class for the table where the player does the body assembling

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AssemblingTable.generated.h"

UCLASS()
class SLICEOFWIFE_API AAssemblingTable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAssemblingTable();

	struct ObjectOnTable
	{
		AActor* object;
		FName tag;
	};

	TArray<ObjectOnTable> bodyParts;

	class UMinigameWidget* widget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UMinigameWidget> WidgetBP;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1"))
	int MinBodyParts = 6;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> TemporarySpawnBody = nullptr;

	UPROPERTY(EditAnywhere)
	FVector SpawnOffset = { 0, 0, 150 };

	UPROPERTY(EditAnywhere)
	FRotator SpawnRotation = { 0, 0, 0 };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool DropToTable(AActor* objectToDrop);
	bool RemoveFromTable(AActor* objectToRemove);

	void StartMinigame();

	void Assemble();

	bool Animate();
};