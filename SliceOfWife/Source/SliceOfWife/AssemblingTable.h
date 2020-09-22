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
	class AFullBody* FinalBody = nullptr;

	UPROPERTY(EditAnywhere)
	FVector SnapPosition = { -90, 0, 105 };

	UPROPERTY(EditAnywhere)
	FRotator SnapRotation = { -90, 0, 90 };

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EBodyPartType> CentralBodyPartType = EBodyPartType::Torso;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UMinigameWidget> MinigameWidget;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool DropToTable(AActor* object, AAssemblingSpot* spot);
	bool CheckBodyPart(ABodyPart* bodyPart, TArray<int>* spotIndexes);
	bool RemoveFromTable(AActor* object);

	bool BeginSewing(AAssemblingSpot* spot);

	void AssembleBodyPart(ABodyPart* bodyPart);
};