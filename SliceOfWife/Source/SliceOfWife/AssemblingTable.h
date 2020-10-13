// The class for the table where the player does the body assembling

#pragma once

#include "EnumsStructs.h"
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

	TArray<class AAssemblingSpot*> assemblingSpots;

	class ABodyPart* centralBodyPart = nullptr;

	UPROPERTY(BlueprintReadOnly)
	class ACreature* FinalBody = nullptr;

	UPROPERTY(EditAnywhere)
	FVector SnapPosition = { -90, 0, 105 };

	UPROPERTY(EditAnywhere)
	FRotator SnapRotation = { -90, 0, 90 };

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EBodyPartType> CentralBodyPartType = EBodyPartType::Torso;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UMinigameWidget> MinigameWidget;

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

	bool DropToTable(AActor* object, AAssemblingSpot* spot);
	bool CanDropToTable(AActor* object, AAssemblingSpot* spot);
	bool CheckValidBodyPart(ABodyPart* bodyPart, TArray<int>* spotIndexes = nullptr);
	bool RemoveFromTable(AActor* object);

	bool BeginSewing(AAssemblingSpot* spot);

	void AssembleBodyPart(ABodyPart* bodyPart);

	bool AnimateBody();

	UFUNCTION(BlueprintCallable)
	bool IsCentreOccupied();
};