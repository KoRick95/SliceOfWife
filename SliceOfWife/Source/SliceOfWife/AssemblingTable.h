// The class for the table where the player does the body assembling

#pragma once

#include "Enums.h"
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

	ABodyPart* centralBodyPart = nullptr;

	class UMinigameWidget* widget;

	UPROPERTY(EditAnywhere)
	FVector SnapPosition = { 0, -100, 100 };

	UPROPERTY(EditAnywhere)
	FRotator SnapRotation = { -90, 0, 90 };

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EBodyPartType> CentralBodyPartType = EBodyPartType::Torso;

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

	bool DropToTable(ABodyPart* bodyPart, AAssemblingSpot* spot);
	bool RemoveFromTable(ABodyPart* bodyPart);

	void StartMinigame();

	UFUNCTION(BlueprintCallable)
	void Assemble(ABodyPart* bodyPart);

	UFUNCTION(BlueprintCallable)
	bool Animate();
};