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

	TArray<FName> bodyPartTags;

	UPROPERTY(EditAnywhere, Category = "Requirements")
	int MinBodyParts = 6;

	UPROPERTY(EditAnywhere, Category = "Requirements")
	TSubclassOf<AActor> TemporarySpawnBody = nullptr;

	UPROPERTY(EditAnywhere, Category = "Requirements")
	FVector SpawnOffset = { 0, 0, 150 };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool DropToTable(AActor* droppedActor);

	bool Animate();
};