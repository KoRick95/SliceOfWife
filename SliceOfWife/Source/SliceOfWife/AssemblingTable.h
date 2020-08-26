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

	TArray<class AAssemblingSpot*> assemblingSpots;

<<<<<<< HEAD
	//TArray<class USceneComponent*> snapComponents;
=======
	TArray<class USceneComponent*> snapComponents;
>>>>>>> parent of 14e10f4... Merge branch 'feature' of https://github.com/KoRick95/SliceOfWife into feature

	TArray<ObjectOnTable> bodyPartsOnTable;

	//TArray<class ABodyPart*> bodyParts;
<<<<<<< HEAD

	USceneComponent* CentralComponent;
=======
>>>>>>> parent of 14e10f4... Merge branch 'feature' of https://github.com/KoRick95/SliceOfWife into feature

	class UMinigameWidget* widget;

	UPROPERTY(EditAnywhere)
	FName CentralBodyPartTag = "Torso";

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1"))
	int MinBodyParts = 6;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> TemporarySpawnBody = nullptr;

	UPROPERTY(EditAnywhere)
	FVector SpawnOffset = { 0, 0, 150 };

	UPROPERTY(EditAnywhere)
	FRotator SpawnRotation = { 0, 0, 0 };

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UMinigameWidget> WidgetBP;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool DropToTable(AActor* objectToDrop);
<<<<<<< HEAD
	bool DropToTableV2(ABodyPart* bodyPart, AAssemblingSpot* spot);
=======
	bool DropToTableV2(ABodyPart* bodyPart);
>>>>>>> parent of 14e10f4... Merge branch 'feature' of https://github.com/KoRick95/SliceOfWife into feature
	bool RemoveFromTable(AActor* objectToRemove);
	bool RemoveFromTableV2(ABodyPart* bodyPart);

	void StartMinigame();

	UFUNCTION(BlueprintCallable)
	void Assemble(ABodyPart* bodyPart);

	UFUNCTION(BlueprintCallable)
	bool Animate();
};