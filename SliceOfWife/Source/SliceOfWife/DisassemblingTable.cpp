#include "DisassemblingTable.h"
#include "Engine.h"

// Sets default values
ADisassemblingTable::ADisassemblingTable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add("DisassemblingTable");
}

// Called when the game starts or when spawned
void ADisassemblingTable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADisassemblingTable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ADisassemblingTable::DropToTable(AActor* body)
{
	if (body->ActorHasTag(TagToCheck))
	{
		// snap the body to the table
		body->SetActorRotation(SnapRotation, ETeleportType::ResetPhysics);
		body->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		body->SetActorLocation(this->GetActorLocation() + SnapPosition);

		this->bodyOnTable = body;
		return true;
	}

	return false;
}

void ADisassemblingTable::Charge()
{
	if (charge < MaxCharge)
	{
		charge++;
	}
	else
	{
		// cut the limbs
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, FString::Printf(TEXT("Charge: %i"), charge));
}

